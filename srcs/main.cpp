#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"

int		main() {
	AChunk * chunk = new Chunk;

	ChunkData * chunkData = chunk->createChunk();

	chunk->updateBlock(chunkData, chunkVec3(2, 3, 4), 3);

	std::cout << "-> " << static_cast<uint16_t>(chunkData->data[2][3][4]) << std::endl;

	delete chunkData;

	delete chunk;
	return 0;
}
