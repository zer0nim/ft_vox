#include "AChunk.hpp"

AChunk::AChunk() {
}

AChunk::AChunk(AChunk const &src) {
	*this = src;
}

AChunk::~AChunk() {
}

AChunk &AChunk::operator=(AChunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void AChunk::updateBlock(ChunkData * chunk, chunkVec3 pos, uint8_t value) const {
	if (chunk->data[pos.x][pos.y][pos.z] == value)
		return;  // unchanged chunk
	chunk->data[pos.x][pos.y][pos.z] = value;
	chunk->isModified = true;
}
