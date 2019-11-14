#include "ChunkManager.hpp"

std::string vecToString(wordIVec3 vec) {
	std::string res;
	res += std::to_string(vec.x);
	res += std::to_string(vec.y);
	res += std::to_string(vec.z);
	return res;
}

ChunkManager::ChunkManager() : _chunkMap(), _chunkActPos(0, 0, 0) {
}

ChunkManager::ChunkManager(ChunkManager const &src) {
	*this = src;
}

ChunkManager::~ChunkManager() {
}

ChunkManager &ChunkManager::operator=(ChunkManager const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void ChunkManager::init(wordFVec3 camPos) {
	_updateChunkPos(camPos);

	for (int32_t x = _chunkActPos.x - CHUNK_SZ_X * RENDER_DISTANCE_CHUNK;
	x <= _chunkActPos.x + CHUNK_SZ_X * RENDER_DISTANCE_CHUNK; x += CHUNK_SZ_X) {
		for (int32_t z = _chunkActPos.z - CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK;
		z <= _chunkActPos.z + CHUNK_SZ_Z * RENDER_DISTANCE_CHUNK; z += CHUNK_SZ_Z) {
			for (int32_t y = 0; y < CHUNK_SZ_Y * MAX_Y_CHUNK; y += CHUNK_SZ_Y) {
				wordIVec3 chunkPos(x, y, z);  // this is the position of the chunk
				AChunk * newChunk = instanciateNewChunk();  // create a chunk with the rihgt type
				newChunk->createChunk();  // init the chunk with the right values
				_insertChunk(chunkPos, newChunk);
			}
		}
	}
}

void ChunkManager::update(wordFVec3 camPos) {
	_updateChunkPos(camPos);
}

void ChunkManager::_updateChunkPos(wordFVec3 pos) {
	_updateChunkPos(wordIVec3(pos));
}
void ChunkManager::_updateChunkPos(wordIVec3 pos) {
	_chunkActPos.x = pos.x - pos.x % 16 + ((pos.x < 0) ? -CHUNK_SZ_X : 0);
	_chunkActPos.y = pos.y - pos.y % 16 + ((pos.y < 0) ? -CHUNK_SZ_Y : 0);
	_chunkActPos.z = pos.z - pos.z % 16 + ((pos.z < 0) ? -CHUNK_SZ_Z : 0);
}

void	ChunkManager::_insertChunk(wordIVec3 chunkPos, AChunk * newChunk) {
	_chunkMap.insert(std::pair<std::string, AChunk*>(vecToString(chunkPos), newChunk));
}

