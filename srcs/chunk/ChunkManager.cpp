#include <sstream>
#include <vector>
#include "ChunkManager.hpp"

std::string vecToString(const wordIVec3 &vec) {
	std::string res;
	res += std::to_string(vec.x) + "_";
	res += std::to_string(vec.y) + "_";
	res += std::to_string(vec.z);
	return res;
}

wordIVec3 stringToVec(const std::string &s) {
    wordIVec3 vec;
    std::stringstream ss(s);
    std::string number;
    std::getline(ss, number, '_');
	vec.x = std::stoi(number);
    std::getline(ss, number, '_');
	vec.y = std::stoi(number);
    std::getline(ss, number, '_');
	vec.z = std::stoi(number);
    return vec;
}

ChunkManager::ChunkManager(std::string const &mapName, TextureManager const &textureManager) :
_mapName(mapName),
_chunkMap(),
_chunkActPos(-1, -1, -1),
_textureManager(textureManager),
_projection() {}

ChunkManager::ChunkManager(ChunkManager const &src) :
_mapName(src.getMapName()),
_textureManager(src.getTextureManager()) {
	*this = src;
}

ChunkManager::~ChunkManager() {
	std::vector<std::string> toDelete;

	// update all chunks
	for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
		toDelete.push_back(it->first);
	}
	// delete all old chunks
	for (auto it = toDelete.begin(); it != toDelete.end(); it++) {
		delete _chunkMap[*it];  // this line auto save chunk
		_chunkMap.erase(*it);
	}
}

ChunkManager &ChunkManager::operator=(ChunkManager const &rhs) {
	if (this != &rhs) {
		_chunkMap = rhs.getChunkMap();
		_projection = rhs.getProjection();
	}
	return *this;
}

void ChunkManager::init(wordFVec3 camPos, glm::mat4 &projection) {
	_projection = projection;
	update(camPos);  // call update once to create the chunks
}

void ChunkManager::update(wordFVec3 &camPos) {
	std::vector<std::string> toDelete;
	wordIVec3 lastChunkPos = _chunkActPos;

	_updateChunkPos(camPos);

	// add new chunks if needed
	if (lastChunkPos != _chunkActPos) {  // if we change the actual chunk
		AChunk * newChunk;
		for (int32_t x = _chunkActPos.x - CHUNK_SZ_X * RENDER_DISTANCE_CHUNK;
		x <= _chunkActPos.x + CHUNK_SZ_X * RENDER_DISTANCE_CHUNK; x += CHUNK_SZ_X) {
			for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK;
			z <= _chunkActPos.z + CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK; z += CHUNK_SZ_Z) {
				for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
					wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
					if (_isChunkExist(chunkPos) == false) {  // if the chunk doesnt exist (for now)
						newChunk = instanciateNewChunk(_textureManager, _projection);  // create a chunk with the rihgt type
						newChunk->createChunk(_mapName, chunkPos);  // init the chunk with the right values
						_insertChunk(chunkPos, newChunk);
					}
				}
			}
		}
	}

	// update all chunks
	for (auto it = _chunkMap.begin(); it != _chunkMap.end(); it++) {
		if (_isInChunkLoaded(stringToVec(it->first))) {
			it->second->update();
		}
		else if (_isInChunkLoadedBorder(stringToVec(it->first)) == false) {  // we need to remove the chunk
			toDelete.push_back(it->first);
		}
	}
	// delete all old chunks
	for (auto it = toDelete.begin(); it != toDelete.end(); it++) {
		delete _chunkMap[*it];
		_chunkMap.erase(*it);
	}
}

void ChunkManager::draw(glm::mat4 view) {
	for (int32_t x = _chunkActPos.x - CHUNK_SZ_X * RENDER_DISTANCE_CHUNK;
	x <= _chunkActPos.x + CHUNK_SZ_X * RENDER_DISTANCE_CHUNK; x += CHUNK_SZ_X) {
		for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK;
		z <= _chunkActPos.z + CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK; z += CHUNK_SZ_Z) {
			for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
				wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
				if (_isChunkExist(chunkPos))
					_chunkMap[vecToString(chunkPos)]->draw(view);
			}
		}
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

void	ChunkManager::_insertChunk(wordIVec3 chunkPos, AChunk * newChunk) {
	_chunkMap.insert(std::pair<std::string, AChunk*>(vecToString(chunkPos), newChunk));
}

bool	ChunkManager::_isInChunkLoaded(wordIVec3 const &chunkPos) const {
	if (chunkPos.x < _chunkActPos.x - RENDER_DISTANCE_CHUNK * CHUNK_SZ_X
	|| chunkPos.x > _chunkActPos.x + RENDER_DISTANCE_CHUNK * CHUNK_SZ_X + CHUNK_SZ_X
	|| chunkPos.z < _chunkActPos.z - RENDER_DISTANCE_CHUNK * CHUNK_SZ_Z
	|| chunkPos.z > _chunkActPos.z + RENDER_DISTANCE_CHUNK * CHUNK_SZ_Z + CHUNK_SZ_Z)
		return false;
	return true;
}

bool	ChunkManager::_isInChunkLoadedBorder(wordIVec3 const &chunkPos) const {
	/*
	return true if the chunk is in loaded chunk or at a dist of 1 to loaded chunks
	*/
	if (chunkPos.x < _chunkActPos.x - (RENDER_DISTANCE_CHUNK + 1) * CHUNK_SZ_X
	|| chunkPos.x > _chunkActPos.x + (RENDER_DISTANCE_CHUNK + 1) * CHUNK_SZ_X + CHUNK_SZ_X
	|| chunkPos.z < _chunkActPos.z - (RENDER_DISTANCE_CHUNK + 1) * CHUNK_SZ_Z
	|| chunkPos.z > _chunkActPos.z + (RENDER_DISTANCE_CHUNK + 1) * CHUNK_SZ_Z + CHUNK_SZ_Z)
		return false;
	return true;
}

bool	ChunkManager::_isChunkExist(std::string const &chunkPos) const {
	return _chunkMap.find(chunkPos) != _chunkMap.end();
}
bool	ChunkManager::_isChunkExist(wordIVec3 const &chunkPos) const {
	return _isChunkExist(vecToString(chunkPos));
}

std::string const &ChunkManager::getMapName() const { return _mapName; }
std::map<std::string, AChunk*>			&ChunkManager::getChunkMap() { return _chunkMap; }
std::map<std::string, AChunk*> const	&ChunkManager::getChunkMap() const { return _chunkMap; }
wordIVec3 const							&ChunkManager::getChunkActPos() const { return _chunkActPos; }
TextureManager const					&ChunkManager::getTextureManager() const { return _textureManager; };
glm::mat4 const							&ChunkManager::getProjection() const { return _projection; };
