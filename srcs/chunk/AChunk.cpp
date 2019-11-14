#include "AChunk.hpp"
#include "Chunk.hpp"

AChunk * instanciateNewChunk() {
	AChunk * newChunk = new Chunk;
	return newChunk;
}

AChunk::AChunk() : _data() {
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

void AChunk::updateBlock(chunkVec3 pos, uint8_t value) {
	if (_data.data[pos.x][pos.y][pos.z] == value)
		return;  // unchanged chunk
	_data.data[pos.x][pos.y][pos.z] = value;
	_data.isModified = true;
}

AChunk::ChunkData const	&AChunk::getData() const { return _data; }
