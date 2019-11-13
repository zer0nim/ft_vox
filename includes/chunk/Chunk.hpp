#pragma once

#include "AChunk.hpp"

class Chunk : public AChunk {
	public:
		Chunk();
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		virtual ChunkData	* createChunk() const;
	protected:
	private:
};
