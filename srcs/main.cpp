#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"

int		main() {
	AChunk * chunk = new Chunk;

	chunk->createChunk();

	chunk->updateBlock(chunkVec3(2, 3, 4), 3);

	std::cout << "-> " << static_cast<uint16_t>(chunk->getData().data[2][3][4]) << std::endl;

	delete chunk;
	return 0;
}
