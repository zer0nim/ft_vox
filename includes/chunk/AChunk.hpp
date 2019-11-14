#ifndef ACHUNK_HPP_
#define ACHUNK_HPP_

#include "commonInclude.hpp"
#include "ft_vox.hpp"


class AChunk {
	public:
		struct ChunkData {
			uint8_t		data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z];
			bool		isModified;

			ChunkData() : isModified(true) {}
		};

		AChunk();
		AChunk(AChunk const &src);
		virtual ~AChunk();

		AChunk &operator=(AChunk const &rhs);

		virtual void	createChunk() = 0;
		virtual void	draw() = 0;

		void		updateBlock(chunkVec3 pos, uint8_t value);
		AChunk::ChunkData const	&getData() const;
	protected:
		ChunkData	_data;
	private:
};

#endif  // ACHUNK_HPP_
