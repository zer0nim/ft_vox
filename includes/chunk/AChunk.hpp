#ifndef ACHUNK_HPP_
#define ACHUNK_HPP_

#include "commonInclude.hpp"
#include "ft_vox.hpp"
#include "TextureManager.hpp"

class AChunk {
	public:
		struct ChunkData {
			uint8_t		data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z];
			bool		isModified;

			ChunkData() : isModified(true) {}
		};

		explicit AChunk(TextureManager const &textureManager, glm::mat4 &projection);
		AChunk(AChunk const &src);
		virtual ~AChunk();

		AChunk &operator=(AChunk const &rhs);

		virtual void	draw(glm::mat4 &view);
		virtual void	update() = 0;
		void			createChunk(wordIVec3 const &chunkPos);
		void			updateBlock(chunkVec3 pos, uint8_t value);
		void			save();

		AChunk::ChunkData const	&getData() const;
		TextureManager const	&getTextureManager() const;
		bool					isModifiedFromBegining() const;

		std::mutex				mutexChunk;

	protected:
		virtual void	_draw(glm::mat4 &view) = 0;
		bool			_createChunkFromFile();
		virtual void	_createChunk();

		ChunkData		_data;  // all chunk data
		wordIVec3		_chunkPos;  // position of the chunk
		std::string		_filename;
		bool			_isModifiedFromBegining;  // true if the chunk was modified
		TextureManager const &_textureManager;
};

AChunk * instanciateNewChunk(TextureManager const &textureManager, glm::mat4 &projection);

#endif  // ACHUNK_HPP_
