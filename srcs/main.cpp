#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"

int		main() {
	ChunkManager * chunkManager = new ChunkManager;

	chunkManager->init(wordFVec3(12.3, 4, -2.3));

	delete chunkManager;
	return 0;
}
