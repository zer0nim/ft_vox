#include <unistd.h>
#include <vector>
#include <cmath>
#include "ChunkManager.hpp"

ChunkManager::ChunkManager(TextureManager const &textureManager, tWinUser *winU) :
toDelete(),
_winU(winU),
_chunkMap(),
_chunkActPos(-1, -1, -1),
_textureManager(textureManager),
_projection(),
_toCreate(),
_nbChunkLoaded{0},
_raycast() {}

ChunkManager::ChunkManager(ChunkManager const &src) :
_textureManager(src.getTextureManager()) {
	*this = src;
}

ChunkManager::~ChunkManager() {
	// update all chunks
	for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
		toDelete.push_back(it->first);
	}
	while (toDelete.empty() == false) {
		delete _chunkMap[toDelete.front()];
		_chunkMap.erase(toDelete.front());
		toDelete.pop_front();
	}
}

ChunkManager &ChunkManager::operator=(ChunkManager const &rhs) {
	if (this != &rhs) {
		_winU = rhs.getWinU();
		_chunkMap = rhs.getChunkMap();
		_projection = rhs.getProjection();
	}
	return *this;
}

void ChunkManager::init(wordFVec3 camPos, glm::mat4 &projection) {
	_projection = projection;
	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		_lastChunkPos[i].x = -1;
		_lastChunkPos[i].y = -1;
		_lastChunkPos[i].z = -1;
		update(camPos, i, LOAD_ALL_BEFORE_OPEN_WINDOW);  // call update once to create the chunks
	}
}

void ChunkManager::update(wordFVec3 &camPos, uint8_t threadID, bool createAll) {
	AChunk *	newChunk;
	if (threadID == 0) {
		std::lock_guard<std::mutex>	guard(s.mutexOthers), guard2(s.mutexCamera);
		_updateChunkPos(camPos);  // update once only
	}

	// add new chunks if needed
	if (_lastChunkPos[threadID] != _chunkActPos) {  // if we change the actual chunk
		_lastChunkPos[threadID] = _chunkActPos;
		int32_t startX = _chunkActPos.x - CHUNK_SZ_X * (s.g.renderDist - 1);
		uint8_t	startID = _getID(startX);
		if (startID > threadID) {
			startX += CHUNK_SZ_X * NB_UPDATE_THREADS;
			startX -= (startID - threadID) * CHUNK_SZ_X;
		}
		else if (startID < threadID) {
			startX += (threadID - startID) * CHUNK_SZ_X;
		}
		for (int32_t x = startX;
		x < _chunkActPos.x + CHUNK_SZ_X * s.g.renderDist; x += CHUNK_SZ_X * NB_UPDATE_THREADS) {
			for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * (s.g.renderDist - 1);
			z < _chunkActPos.z + CHUNK_SZ_Z * s.g.renderDist; z += CHUNK_SZ_Z) {
				for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
					wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
					bool exist;
				    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
						exist = _isChunkExist(chunkPos);
					}
					if (exist == false) {  // if the chunk doesnt exist (for now)
						std::lock_guard<std::mutex>	guard(s.mutexToCreate);
						auto it = std::find(_toCreate[threadID].begin(), _toCreate[threadID].end(), chunkPos);
						if (it == _toCreate[threadID].end()) {  // if the chunk is not already in creation queue
							_toCreate[threadID].push_back(chunkPos);
						}
					}
				}
			}
		}
	}

	int			i = 0;
	wordIVec3	chunkPos;
	while (i < MAX_CREATED_CHUNK_UPDATE_COUNT) {
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap), guard2(s.mutexToCreate);
			// skip old chunks
			while (_toCreate[threadID].empty() == false && _isInChunkLoaded(_toCreate[threadID].front()) == false) {
				_toCreate[threadID].pop_front();
			}
			if (_toCreate[threadID].empty())
				break;
			chunkPos = _toCreate[threadID].front();
			_toCreate[threadID].pop_front();
		}
		bool exist;
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			exist = _isChunkExist(chunkPos);
		}
		if (exist == false) {  // if the chunk doesnt exist (for now)
			newChunk = instanciateNewChunk(_textureManager, _projection);  // create a chunk with the rihgt type
			newChunk->createChunk(chunkPos);  // init the chunk with the right values
			newChunk->update();
		    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
				_chunkMap[chunkPos] = newChunk;
			}
		}
		if (ENABLE_MAX_CREATED_CHUNK_UPDATE && createAll == false)
			i++;
	}

	// update all chunks
	uint32_t	chunkLoaded = 0;
    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
		for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
			if (_getID(it->first) == threadID) {
				chunkLoaded++;
				if (_isInChunkLoaded(it->first)) {
				    { std::lock_guard<std::mutex>	guard(it->second->mutexChunk);
						it->second->update();
					}
				}
				else {  // we need to remove the chunk
				    { std::lock_guard<std::mutex>	guard(s.mutexToDelete);
						toDelete.push_back(it->first);
					}
					if (s.g.files.saveAllChunks || it->second->isModifiedFromBegining())  // save (if needed)
						it->second->save();
				}
			}
		}
	}
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		_nbChunkLoaded[threadID] = chunkLoaded;
	}
}

void ChunkManager::draw(glm::mat4 view, Camera *cam) {
	glm::vec3	chunkSize(CHUNK_SZ_X, CHUNK_SZ_Y, CHUNK_SZ_Z);
	uint32_t	chunkRendered = 0;

	for (int32_t x = _chunkActPos.x - CHUNK_SZ_X * (s.g.renderDist - 1);
	x < _chunkActPos.x + CHUNK_SZ_X * s.g.renderDist; x += CHUNK_SZ_X) {
		for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * (s.g.renderDist - 1);
		z < _chunkActPos.z + CHUNK_SZ_Z * s.g.renderDist; z += CHUNK_SZ_Z) {
			for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
				wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
				bool exist;
			    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
					exist = _isChunkExist(chunkPos);
				}
				if (exist) {  // if the chunk exist
					// if inside the camera
					bool	frclInside;
				    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
						frclInside = FRCL_IS_INSIDE(cam->frustumCullingCheckCube(chunkPos, chunkSize));
					}
					if (frclInside) {
						++chunkRendered;
						std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
						_chunkMap[chunkPos]->draw(view);
					}
				}
			}
		}
	}
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		_nbChunkRendered = chunkRendered;
	}
}

void ChunkManager::saveAndQuit() {
	std::lock_guard<std::mutex>	guard(s.mutexChunkMap), guard2(s.mutexToDelete);
	for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
		toDelete.push_back(it->first);
		if (s.g.files.saveAllChunks || it->second->isModifiedFromBegining())  // save (if needed)
			it->second->save();
	}
	while (toDelete.empty() == false) {
		delete _chunkMap[toDelete.front()];
		_chunkMap.erase(toDelete.front());
		toDelete.pop_front();
	}
}

void ChunkManager::destroyBlock() {
    { std::lock_guard<std::mutex>	guard(s.mutexOthers), guard2(s.mutexChunkMap);
		if (_raycast.isBlockSelected) {
			updateBlock(_raycast.selectedBlock, 0);
			_raycast.isBlockSelected = false;
			_raycast.blockType = 0;
		}
	}
}

void ChunkManager::updateRaycast() {
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		if (_raycast.isBlockSelected) {
		    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
				if (getBlock(_raycast.selectedBlock) != 0)
					updateBlock(_raycast.selectedBlock, _raycast.blockType);
			}
		}
	}

	glm::vec3	point;
	float		start = 0.6;
	float		end = 0.99;
	float		step = 0.001;
	uint8_t		block = 0;

	for (float dist = start; dist <= end; dist += step) {
		point = glm::unProject(glm::vec3(s.g.screen.width / 2, s.g.screen.height / 2, dist),
							   _winU->cam->getViewMatrix(),
							   _projection,
							   glm::vec4(0, 0, s.g.screen.width, s.g.screen.height));
		block = getBlock(point);
		if (block > 0)
			break;
	}

    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		if (block > 0) {
			_raycast.isBlockSelected = true;
			_raycast.selectedBlock = wordIVec3(point);
			_raycast.blockType = block;
	////////////////////////////////////////
		    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
				updateBlock(_raycast.selectedBlock, TextureManager::blocksNames["bedrock"]);
			}
	////////////////////////////////////////
		}
		else {
			_raycast.isBlockSelected = false;
			_raycast.blockType = 0;
		}
	}
}

void ChunkManager::_updateChunkPos(wordFVec3 const &pos) {
	_updateChunkPos(wordIVec3(pos));
}
void ChunkManager::_updateChunkPos(wordIVec3 const &pos) {
	_chunkActPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0 && pos.x % CHUNK_SZ_X != 0) ? -CHUNK_SZ_X : 0);
	_chunkActPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0 && pos.y % CHUNK_SZ_Y != 0) ? -CHUNK_SZ_Y : 0);
	_chunkActPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0 && pos.z % CHUNK_SZ_Z != 0) ? -CHUNK_SZ_Z : 0);
}

bool	ChunkManager::_isInChunkLoaded(wordIVec3 const &chunkPos) const {
	if (chunkPos.x <= _chunkActPos.x - s.g.renderDist * CHUNK_SZ_X
	|| chunkPos.x >= _chunkActPos.x + s.g.renderDist * CHUNK_SZ_X
	|| chunkPos.z <= _chunkActPos.z - s.g.renderDist * CHUNK_SZ_Z
	|| chunkPos.z >= _chunkActPos.z + s.g.renderDist * CHUNK_SZ_Z)
		return false;
	return true;
}

bool	ChunkManager::_isChunkExist(wordIVec3 const &chunkPos) const {
	return _chunkMap.find(chunkPos) != _chunkMap.end();
}
uint8_t	ChunkManager::_getID(wordIVec3 const &chunkPos) const {
	return _getID(chunkPos.x);
}
uint8_t	ChunkManager::_getID(int32_t const x) const {
	if (x >= 0)
		return (x / CHUNK_SZ_X) % NB_UPDATE_THREADS;
	return (NB_UPDATE_THREADS - (-x / CHUNK_SZ_X) % NB_UPDATE_THREADS) % NB_UPDATE_THREADS;
}

tWinUser								*ChunkManager::getWinU() { return _winU; }
tWinUser								*ChunkManager::getWinU() const { return _winU; }
std::map<wordIVec3, AChunk*>			&ChunkManager::getChunkMap() { return _chunkMap; }
std::map<wordIVec3, AChunk*> const		&ChunkManager::getChunkMap() const { return _chunkMap; }
wordIVec3 const							&ChunkManager::getChunkActPos() const { return _chunkActPos; }
TextureManager const					&ChunkManager::getTextureManager() const { return _textureManager; };
glm::mat4 const							&ChunkManager::getProjection() const { return _projection; };
uint32_t								ChunkManager::getNbChunkLoaded() const {
	uint32_t ret = 0;
	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		ret += _nbChunkLoaded[i];
	}
	return ret;
}
uint32_t	ChunkManager::getNbChunkRendered() const { return _nbChunkRendered; }
uint8_t		ChunkManager::getBlock(wordFVec3 pos) const {
	return getBlock(static_cast<wordIVec3>(pos));
}
uint8_t		ChunkManager::getBlock(wordIVec3 pos) const {
	uint8_t		ret = 0;
	wordIVec3	chunkPos;
	chunkPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0 && pos.x % CHUNK_SZ_X != 0) ? -CHUNK_SZ_X : 0);
	chunkPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0 && pos.y % CHUNK_SZ_Y != 0) ? -CHUNK_SZ_Y : 0);
	chunkPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0 && pos.z % CHUNK_SZ_Z != 0) ? -CHUNK_SZ_Z : 0);

	if (_isChunkExist(chunkPos)) {
		AChunk * chunk = _chunkMap.at(chunkPos);
	    { std::lock_guard<std::mutex>	guard(chunk->mutexChunk);
			ret = chunk->getData().data[std::abs(pos.x - chunkPos.x)]
									[std::abs(pos.y - chunkPos.y)]
									[std::abs(pos.z - chunkPos.z)];
		}
	}
	return ret;
}
void	ChunkManager::updateBlock(wordFVec3 pos, uint8_t value) const {
	updateBlock(static_cast<wordIVec3>(pos), value);
}
void	ChunkManager::updateBlock(wordIVec3 pos, uint8_t value) const {
	wordIVec3	chunkPos;
	chunkPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0 && pos.x % CHUNK_SZ_X != 0) ? -CHUNK_SZ_X : 0);
	chunkPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0 && pos.y % CHUNK_SZ_Y != 0) ? -CHUNK_SZ_Y : 0);
	chunkPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0 && pos.z % CHUNK_SZ_Z != 0) ? -CHUNK_SZ_Z : 0);

	if (_isChunkExist(chunkPos)) {
		AChunk * chunk = _chunkMap.at(chunkPos);
		chunkVec3	blockPos = chunkVec3(std::abs(pos.x - chunkPos.x),
										 std::abs(pos.y - chunkPos.y),
										 std::abs(pos.z - chunkPos.z));
	    { std::lock_guard<std::mutex>	guard(chunk->mutexChunk);
			chunk->updateBlock(blockPos, value);
		}
	}
}
