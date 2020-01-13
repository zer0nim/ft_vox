#include <unistd.h>
#include <vector>
#include <cmath>
#include "ChunkManager.hpp"
#include "CameraSurvival.hpp"

const float	ChunkManager::_borderVertices[] = {
	0, 0, 0,
	0, 0, 1,

	1, 0, 0,
	1, 0, 1,

	0, 0, 0,
	1, 0, 0,

	0, 0, 1,
	1, 0, 1,

	0, 1, 0,
	0, 1, 1,

	1, 1, 0,
	1, 1, 1,

	0, 1, 0,
	1, 1, 0,

	0, 1, 1,
	1, 1, 1,

	0, 0, 0,
	0, 1, 0,

	0, 0, 1,
	0, 1, 1,

	1, 0, 0,
	1, 1, 0,

	1, 0, 1,
	1, 1, 1,
};

ChunkManager::ChunkManager(TextureManager const &textureManager, tWinUser *winU) :
toDelete(),
raycast(),
_winU(winU),
_chunkMap(),
_chunkActPos(-1, -1, -1),
_textureManager(textureManager),
_toCreate(),
_nbChunkLoaded(0),
_borderShader(nullptr) {
	_lastDestroyed = getMs();
	_lastPut = getMs();
}

ChunkManager::ChunkManager(ChunkManager const &src) :
_textureManager(src.getTextureManager()) {
	*this = src;
}

ChunkManager::~ChunkManager() {
	std::lock_guard<std::mutex>	guard(s.mutexChunkMap), guard2(s.mutexToDelete);
	// update all chunks
	for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
		toDelete.push_back(it->first);
	}
	while (toDelete.empty() == false) {
		delete _chunkMap[toDelete.front()];
		_chunkMap.erase(toDelete.front());
		toDelete.pop_front();
	}

	glDeleteVertexArrays(1, &_borderShaderVAO);
	glDeleteBuffers(1, &_borderShaderVBO);
	delete _borderShader;
}

ChunkManager &ChunkManager::operator=(ChunkManager const &rhs) {
	if (this != &rhs) {
		_winU = rhs.getWinU();
		std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
		_chunkMap = rhs.getChunkMap();
	}
	return *this;
}

void ChunkManager::init(wordFVec3 camPos, glm::mat4 &projection) {
	_projection = projection;
	_borderShader = new Shader("shaders/blockBorder_vs.glsl", "shaders/blockBorder_fs.glsl");
	_borderShader->use();
	_borderShader->setMat4("projection", _projection);
    glGenVertexArrays(1, &_borderShaderVAO);
    glGenBuffers(1, &_borderShaderVBO);

    glBindBuffer(GL_ARRAY_BUFFER, _borderShaderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkManager::_borderVertices), ChunkManager::_borderVertices, GL_STATIC_DRAW);

    glBindVertexArray(_borderShaderVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
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
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers), guard2(s.mutexCamera);
			_updateChunkPos(camPos);  // update once only
		}
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			_nbChunkLoaded = _chunkMap.size();  // update only once
		}
	}

	uint32_t startX;  // pos of first chunk (in all thread)
	uint32_t startXthread;  // pos of first chunk (in this thread only)
	uint32_t startZ;  // pos of first chunk (in all thread)

	// startX
	startX = _chunkActPos.x - CHUNK_SZ_X * (s.g.renderDist - 1);
	// startXthread
	startXthread = startX;
	uint8_t	startID = _getID(startXthread);
	if (startID > threadID) {
		startXthread += CHUNK_SZ_X * NB_UPDATE_THREADS;
		startXthread -= (startID - threadID) * CHUNK_SZ_X;
	}
	else if (startID < threadID) {
		startXthread += (threadID - startID) * CHUNK_SZ_X;
	}
	// startZ
	startZ = _chunkActPos.z - CHUNK_SZ_Z * (s.g.renderDist - 1);

	// add new chunks if needed
	if (_lastChunkPos[threadID] != _chunkActPos) {  // if we change the actual chunk
		_lastChunkPos[threadID] = _chunkActPos;
		for (int32_t x = startXthread; x < _chunkActPos.x + CHUNK_SZ_X * s.g.renderDist;
		x += CHUNK_SZ_X * NB_UPDATE_THREADS) {
			for (int32_t z = startZ; z < _chunkActPos.z + CHUNK_SZ_Z * s.g.renderDist; z += CHUNK_SZ_Z) {
				for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
					wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
					bool exist;
				    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
						exist = isChunkExist(chunkPos);
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

	// update render
	#if REMOVE_CHUNKS_BORDERS
		int32_t	nbLoaded;
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
			nbLoaded = getNbChunkLoaded();
		}
		if (nbLoaded >= (s.g.renderDist * 2 - 1) * (s.g.renderDist * 2 - 1) * 2) {  // if all chunks are created
			uint8_t	nbUpdated = 0;
			for (int32_t x = startXthread; x < _chunkActPos.x + CHUNK_SZ_X * s.g.renderDist;
			x += CHUNK_SZ_X * NB_UPDATE_THREADS) {
				for (int32_t z = startZ; z < _chunkActPos.z + CHUNK_SZ_Z * s.g.renderDist; z += CHUNK_SZ_Z) {
					for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
						wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
						bool exist;
					    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
							exist = isChunkExist(chunkPos);
						}
						if (exist) {
							std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
							std::lock_guard<std::mutex>	guard2(_chunkMap[chunkPos]->mutexChunk);
							if (_chunkMap[chunkPos]->renderUpdate(startX, startZ)) {
								nbUpdated++;
							}
						}
						if (nbUpdated >= MAX_RENDER_CHUNK_UPDATE_COUNT) {
							break;
						}
					}
					if (nbUpdated >= MAX_RENDER_CHUNK_UPDATE_COUNT) {
						break;
					}
				}
				if (nbUpdated >= MAX_RENDER_CHUNK_UPDATE_COUNT) {
					break;
				}
			}
		}
	#endif

	// create chunks
	int			i = 0;
	wordIVec3	chunkPos;
	while (i < MAX_CREATED_CHUNK_UPDATE_COUNT) {
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap), guard2(s.mutexToCreate);
			#if SMART_LOAD_ORDER
				if (_toCreate[threadID].size() == 0)
					break;
				uint32_t	minDist = 100000;
				uint32_t	idMinDist = 0;
				// get the closest chunk
				for (uint32_t j = 0; j < _toCreate[threadID].size(); j++) {
					uint32_t dist = std::abs(_toCreate[threadID][j].x - _chunkActPos.x)
									+ std::abs(_toCreate[threadID][j].z - _chunkActPos.z);
					if (dist < minDist) {
						minDist = dist;
						idMinDist = j;
					}
				}
				chunkPos = _toCreate[threadID][idMinDist];
				_toCreate[threadID].erase(_toCreate[threadID].begin() + idMinDist);
			#else
				// skip old chunks
				while (_toCreate[threadID].empty() == false && _isInChunkLoaded(_toCreate[threadID].front()) == false) {
					_toCreate[threadID].pop_front();
				}
				if (_toCreate[threadID].empty())
					break;
				chunkPos = _toCreate[threadID].front();
				_toCreate[threadID].pop_front();
			#endif
		}
		bool exist;
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			exist = isChunkExist(chunkPos);
			if (_isInChunkLoaded(chunkPos) == false) {
				exist = true;
			}
		}
		if (exist == false) {  // if the chunk doesnt exist (for now)
			// create a chunk with the rihgt type
			newChunk = instanciateNewChunk(_textureManager, *this);
			newChunk->createChunk(chunkPos);  // init the chunk with the right values
			// /!\ WARNING /!\ this update will call mutexChunkMap
			newChunk->update(false);
		    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
				_chunkMap[chunkPos] = newChunk;
			}
		}
		if (ENABLE_MAX_CREATED_CHUNK_UPDATE && createAll == false)
			i++;
	}

	// update all chunks
	std::map<wordIVec3, AChunk*>::iterator it;
	std::map<wordIVec3, AChunk*>::iterator end;
    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
		it = _chunkMap.begin();
		end = _chunkMap.end();
	}
	wordIVec3	actChunkPos;
	AChunk		*actChunk;
	while (it != end) {
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			actChunkPos = it->first;
			actChunk = it->second;
		}
		if (_getID(actChunkPos) == threadID) {
			if (_isInChunkLoaded(actChunkPos)) {
			    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap), guard2(actChunk->mutexChunk);
					// /!\ WARNING /!\ this update will call mutexChunkMap
					actChunk->update();
				}
			}
			else {  // we need to remove the chunk
			    { std::lock_guard<std::mutex>	guard(s.mutexToDelete);
					toDelete.push_back(actChunkPos);
				}
				if (s.g.files.saveAllChunks || actChunk->isModifiedFromBegining())  // save (if needed)
				    { std::lock_guard<std::mutex>	guard(actChunk->mutexChunk);
						actChunk->save();
					}
			}
		}
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			it++;
		}
	}
}

void ChunkManager::draw(Camera *cam) {
	CAMERA_VEC3	chunkSize(CHUNK_SZ_X, CHUNK_SZ_Y, CHUNK_SZ_Z);
	uint32_t	chunkRendered = 0;
	uint32_t	squareRendered = 0;
	AChunk		*chunk;

	wordIVec3	chunkOffset(0, 0, 0);  // an offset to avoid precision pbs
	CAMERA_MAT4	tmpView;
	CAMERA_VEC3	tmpCamPos;

	tmpCamPos = cam->pos;
	if (tmpCamPos.x < -1000 || tmpCamPos.x > 1000) {
		chunkOffset.x = static_cast<int32_t>(tmpCamPos.x) / 1000 * 1000;
		tmpCamPos.x = std::fmod(tmpCamPos.x, 1000);
	}
	if (tmpCamPos.z < -1000 || tmpCamPos.z > 1000) {
		chunkOffset.z = static_cast<int32_t>(tmpCamPos.z) / 1000 * 1000;
		tmpCamPos.z = std::fmod(tmpCamPos.z, 1000);
	}
	tmpView = glm::lookAt(tmpCamPos, tmpCamPos + cam->front, cam->up);

	for (int32_t x = _chunkActPos.x - CHUNK_SZ_X * (s.g.renderDist - 1);
	x < _chunkActPos.x + CHUNK_SZ_X * s.g.renderDist; x += CHUNK_SZ_X) {
		for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * (s.g.renderDist - 1);
		z < _chunkActPos.z + CHUNK_SZ_Z * s.g.renderDist; z += CHUNK_SZ_Z) {
			for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
				wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
				bool exist;
			    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
					exist = isChunkExist(chunkPos);
				}
				if (exist) {  // if the chunk exist
					// if inside the camera
					bool	frclInside;
				    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
						frclInside = FRCL_IS_INSIDE(cam->frustumCullingCheckCube(chunkPos, chunkSize));
					}
					if (frclInside) {
						++chunkRendered;
					    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
							chunk = _chunkMap[chunkPos];
						}
						squareRendered += chunk->getNbSquareRendered();
						chunk->draw(tmpView, chunkOffset, tmpCamPos);
					}
				}
			}
		}
	}
	bool blockSelected;
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		blockSelected = raycast.isBlockSelected;
	}
	if (blockSelected) {
		glm::mat4 model;
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(raycast.selectedBlock - chunkOffset));
		}
		_borderShader->use();
		_borderShader->setMat4("view", tmpView);
		_borderShader->setMat4("model", model);
		glBindVertexArray(_borderShaderVAO);
		glDrawArrays(GL_LINES, 0, 24);
	}
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		_nbChunkRendered = chunkRendered;
		_nbSquareRendrered = squareRendered;
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
	std::chrono::milliseconds	time = getMs();
	bool						canChangeBlock = false;
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		if ((time - _lastDestroyed).count() > s.g.player.delayDestroyMs)
			canChangeBlock = true;
	}
	if (canChangeBlock) {
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers), guard2(s.mutexChunkMap);
			if (raycast.isBlockSelected) {
				if (_winU->cam == _winU->camSurv && raycast.blockType == TextureManager::blocksNames["bedrock"])
					return;  // you can't destroy blocks in survival mode
				updateBlock(raycast.selectedBlock, 0);
				raycast.isBlockSelected = false;
				raycast.blockType = 0;
				_lastDestroyed = time;
			}
		}
	}
}

void ChunkManager::putBlock(uint8_t type) {
	std::chrono::milliseconds	time = getMs();
	bool						canChangeBlock = false;
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		if ((time - _lastPut).count() > s.g.player.delayPutMs)
			canChangeBlock = true;
	}
	if (canChangeBlock) {
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers), guard2(s.mutexChunkMap);
			if (raycast.isBeforeBlock) {
				if (s.m.gamemode == GAMEMODE_SURVIVAL
				&& dynamic_cast<CameraSurvival *>(_winU->cam)->isOnBlock(raycast.beforeSelectedBlock))
					return;  // if try to put a block on the player
				updateBlock(raycast.beforeSelectedBlock, type);
				raycast.isBlockSelected = false;
				raycast.isBeforeBlock = false;
				raycast.blockType = 0;
				_lastPut = time;
			}
		}
	}
}

void ChunkManager::updateRaycast() {
	wordIVec3	point;
	wordIVec3	pointLastBlock;
	bool		isBeforeBlock = false;
	float		start = 0.6;
	float		end = 0.99;
	float		stepDivVal = 0.97;
	float		step = 0.0005;
	uint8_t		block = 0;

	wordIVec3	chunkOffset(0, 0, 0);  // an offset to avoid precision pbs
	glm::mat4	tmpViewf;
	CAMERA_VEC3	tmpCamPos;

	tmpCamPos = _winU->cam->pos;
	if (tmpCamPos.x < -1000 || tmpCamPos.x > 1000) {
		chunkOffset.x = static_cast<int32_t>(tmpCamPos.x) / 1000 * 1000;
		tmpCamPos.x = std::fmod(tmpCamPos.x, 1000);
	}
	if (tmpCamPos.z < -1000 || tmpCamPos.z > 1000) {
		chunkOffset.z = static_cast<int32_t>(tmpCamPos.z) / 1000 * 1000;
		tmpCamPos.z = std::fmod(tmpCamPos.z, 1000);
	}
	tmpViewf = glm::lookAt(tmpCamPos, tmpCamPos + _winU->cam->front, _winU->cam->up);

	for (float dist = start; dist <= end; dist += step) {
		if (dist >= stepDivVal) {
			stepDivVal = 2;  // to enter in this condition only once
			step /= 5;
		}
		glm::vec3 tmpPoint = glm::unProject(glm::vec3(s.g.screen.width / 2, s.g.screen.height / 2, dist),
							   tmpViewf,
							   _projection,
							   glm::vec4(0, 0, s.g.screen.width, s.g.screen.height));
		if (tmpPoint.x < 0) tmpPoint.x -= 1;
		if (tmpPoint.y < 0) tmpPoint.y -= 1;
		if (tmpPoint.z < 0) tmpPoint.z -= 1;
		point = wordIVec3(tmpPoint) + chunkOffset;
	    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
			block = getBlock(point);
		}
		if (dist > start) {
			if (pointLastBlock != point && block == 0) {
				pointLastBlock = point;
				isBeforeBlock = true;
			}
		}
		else {
			pointLastBlock = point;
		}
		if (block > 0)
			break;
	}

    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		if (block > 0) {
			raycast.isBlockSelected = true;
			raycast.selectedBlock = point;
			raycast.blockType = block;
			raycast.isBeforeBlock = true;
			raycast.beforeSelectedBlock = pointLastBlock;
		}
		else {
			raycast.isBlockSelected = false;
			raycast.isBeforeBlock = false;
			raycast.blockType = 0;
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

bool	ChunkManager::isChunkExist(wordIVec3 const &chunkPos) const {
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
uint32_t								ChunkManager::getNbChunkLoaded() const { return _nbChunkLoaded; }
uint32_t								ChunkManager::getNbChunkRendered() const { return _nbChunkRendered; }
uint32_t								ChunkManager::getNbSquareRendered() const { return _nbSquareRendrered; }
uint8_t		ChunkManager::getBlock(wordFVec3 pos) const {
	if (pos.x < 0) pos.x -= 1;
	if (pos.y < 0) pos.y -= 1;
	if (pos.z < 0) pos.z -= 1;
	return getBlock(static_cast<wordIVec3>(pos));
}
uint8_t		ChunkManager::getBlock(wordIVec3 pos) const {
	uint8_t		ret = 0;
	wordIVec3	chunkPos;
	chunkPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0 && pos.x % CHUNK_SZ_X != 0) ? -CHUNK_SZ_X : 0);
	chunkPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0 && pos.y % CHUNK_SZ_Y != 0) ? -CHUNK_SZ_Y : 0);
	chunkPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0 && pos.z % CHUNK_SZ_Z != 0) ? -CHUNK_SZ_Z : 0);

	if (isChunkExist(chunkPos)) {
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
	if (pos.x < 0) pos.x -= 1;
	if (pos.y < 0) pos.y -= 1;
	if (pos.z < 0) pos.z -= 1;
	updateBlock(static_cast<wordIVec3>(pos), value);
}
void	ChunkManager::updateBlock(wordIVec3 pos, uint8_t value) const {
	wordIVec3	chunkPos;
	chunkPos.x = pos.x - pos.x % CHUNK_SZ_X + ((pos.x < 0 && pos.x % CHUNK_SZ_X != 0) ? -CHUNK_SZ_X : 0);
	chunkPos.y = pos.y - pos.y % CHUNK_SZ_Y + ((pos.y < 0 && pos.y % CHUNK_SZ_Y != 0) ? -CHUNK_SZ_Y : 0);
	chunkPos.z = pos.z - pos.z % CHUNK_SZ_Z + ((pos.z < 0 && pos.z % CHUNK_SZ_Z != 0) ? -CHUNK_SZ_Z : 0);

	if (isChunkExist(chunkPos)) {
		AChunk * chunk = _chunkMap.at(chunkPos);
		chunkVec3	blockPos = chunkVec3(std::abs(pos.x - chunkPos.x),
										 std::abs(pos.y - chunkPos.y),
										 std::abs(pos.z - chunkPos.z));
	    { std::lock_guard<std::mutex>	guard(chunk->mutexChunk);
			chunk->updateBlock(blockPos, value);
		}
	}
}
