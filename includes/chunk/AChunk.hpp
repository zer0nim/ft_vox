#pragma once

#include "commonInclude.hpp"
#include "ft_vox.hpp"

struct ChunkData {
	uint8_t		data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z];
	bool		isModified;

	ChunkData() : isModified(false) {}
};

class AChunk {
	public:
		AChunk();
		AChunk(AChunk const &src);
		virtual ~AChunk();

		AChunk &operator=(AChunk const &rhs);

		virtual ChunkData	*createChunk() const = 0;

		void	updateBlock(ChunkData *chunk, chunkVec3 pos, uint8_t value) const;
	protected:
	private:
};
