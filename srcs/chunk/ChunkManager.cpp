#include <unistd.h>
#include <vector>
#include "ChunkManager.hpp"

ChunkManager::ChunkManager(TextureManager const &textureManager, tWinUser *winU) :
toDelete(),
_winU(winU),
_chunkMap(),
_chunkActPos(-1, -1, -1),
_textureManager(textureManager),
_toCreate(),
_nbChunkLoaded{0} {}

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
	}
	return *this;
}

void ChunkManager::init(wordFVec3 camPos) {
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
		std::lock_guard<std::mutex>	guard(s.mutexOthers);
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
			// create a chunk with the rihgt type
			newChunk = instanciateNewChunk(_textureManager);
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
					while (it->second->isDrawing) {
						usleep(10);
					}
					it->second->isUpdating = true;
					it->second->update();
					it->second->isUpdating = false;
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
					if (FRCL_IS_INSIDE(cam->frustumCullingCheckCube(chunkPos, chunkSize))) {
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

void ChunkManager::_updateChunkPos(wordFVec3 const &pos) {
	_updateChunkPos(wordIVec3(pos));
}
void ChunkManager::_updateChunkPos(wordIVec3 const &pos) {
	_chunkActPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0) ? -CHUNK_SZ_X : 0);
	_chunkActPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0) ? -CHUNK_SZ_Y : 0);
	_chunkActPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0) ? -CHUNK_SZ_Z : 0);
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
std::map<wordIVec3, AChunk*> const	&ChunkManager::getChunkMap() const { return _chunkMap; }
wordIVec3 const							&ChunkManager::getChunkActPos() const { return _chunkActPos; }
TextureManager const					&ChunkManager::getTextureManager() const { return _textureManager; };
uint32_t								ChunkManager::getNbChunkLoaded() const {
	uint32_t ret = 0;
	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		ret += _nbChunkLoaded[i];
	}
	return ret;
}
uint32_t								ChunkManager::getNbChunkRendered() const { return _nbChunkRendered; }
