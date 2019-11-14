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

		virtual void	update() = 0;
		void			createChunk(std::string const &mapName, wordIVec3 const &chunkPos);
		void			createChunk(std::string const &mapName, std::string const &chunkPos);
		void			updateBlock(chunkVec3 pos, uint8_t value);
		void			save();

		AChunk::ChunkData const	&getData() const;

	protected:
		bool			_createChunkFromFile();
		virtual void	_createChunk() = 0;
		ChunkData		_data;
		std::string		_filename;
		bool			_isModifiedFromBegining;  // true if the chunk was modified
};

AChunk * instanciateNewChunk();

#endif  // ACHUNK_HPP_
