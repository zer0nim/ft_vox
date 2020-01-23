#ifndef ACHUNK_HPP_
#define ACHUNK_HPP_

#include <memory>

#include "commonInclude.hpp"
#include "ft_vox.hpp"
#include "TextureManager.hpp"

class AChunk {
	public:
		struct ChunkData {
			uint8_t		data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z];
			bool		isModified;
			uint32_t	nbBlocks;

			ChunkData() : isModified(true), nbBlocks(0) {}
		};

		explicit AChunk(TextureManager const &textureManager, ChunkManager &chunkManager);
		AChunk(AChunk const &src);
		virtual ~AChunk();

		AChunk &operator=(AChunk const &rhs);

		virtual void	draw(CAMERA_MAT4 &view, wordIVec3 &chunkOffset, CAMERA_VEC3 &pos, \
		float nightProgress, bool pointLight);
		virtual void	update(bool isChunkMapMutexed = true) = 0;
		virtual bool	renderUpdate(int32_t startX, int32_t startZ) = 0;  // return true if updated
		void			createChunk(wordIVec3 const &chunkPos);
		void			updateBlock(chunkVec3 pos, uint8_t value);
		void			save();

		static Shader			&getShader();
		AChunk::ChunkData const	&getData() const;
		AChunk::ChunkData		&getData();
		TextureManager const	&getTextureManager() const;
		ChunkManager			&getChunkManager() const;
		bool					isModifiedFromBegining() const;
		virtual uint32_t		getNbSquareRendered() const { return 0; }
		static void				deleteShader();

		mutable std::mutex		mutexChunk;

	protected:
		struct ShaderData {
			Shader		*shader;
			uint32_t	vao;

			ShaderData(std::string const vs, std::string const fs, std::string const gs = "") {
				if (gs.empty()) {
					shader = new Shader(vs, fs);
				}
				else {
					shader = new Shader(vs, fs, gs);
				}
				shader->use();
				glGenVertexArrays(1, &vao);
				shader->unuse();
			}
			~ShaderData() {
				shader->use();
				glDeleteVertexArrays(1, &vao);
				shader->unuse();
				delete shader;
			}
		};
		static std::unique_ptr<ShaderData>	_shaderData;

		virtual void	_draw(CAMERA_MAT4 &view, wordIVec3 &chunkOffset, CAMERA_VEC3 &pos, \
		float nightProgress, bool pointLight) = 0;
		bool			_createChunkFromFile();
		virtual void	_createChunk();

		ChunkData		_data;  // all chunk data
		wordIVec3		_chunkPos;  // position of the chunk
		std::string		_filename;
		bool			_isModifiedFromBegining;  // true if the chunk was modified
		int				_needRenderUpdate;
		TextureManager const &_textureManager;
		ChunkManager	&_chunkManager;
};

AChunk * instanciateNewChunk(TextureManager const &textureManager, ChunkManager &chunkManager);

#endif  // ACHUNK_HPP_
