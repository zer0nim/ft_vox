#include "Chunk.hpp"

Chunk::Chunk() : AChunk() {
}

Chunk::Chunk(Chunk const &src) : AChunk(src) {
	*this = src;
}

Chunk::~Chunk() {
}

Chunk &Chunk::operator=(Chunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

ChunkData	*Chunk::createChunk() const {
	ChunkData * newChunk = new ChunkData;

	for (uint8_t x = 0; x < CHUNK_SZ_X; x++) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; y++) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; z++) {
				newChunk->data[x][y][z] = 0;
			}
		}
	}

	return newChunk;
}
