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

			ChunkData() : isModified(true) {}
		};

		explicit AChunk(TextureManager const &textureManager);
		AChunk(AChunk const &src);
		virtual ~AChunk();

		AChunk &operator=(AChunk const &rhs);

		virtual void	draw(glm::mat4 &view, glm::vec3 &pos);
		virtual void	update() = 0;
		void			createChunk(wordIVec3 const &chunkPos);
		void			updateBlock(chunkVec3 pos, uint8_t value);
		void			save();

		static Shader			&getShader();
		AChunk::ChunkData const	&getData() const;
		TextureManager const	&getTextureManager() const;
		bool					isModifiedFromBegining() const;

		std::mutex				mutexChunk;

	protected:
		struct ShaderData {
			Shader		*shader;
			u_int32_t	vbo;
			u_int32_t	vao;
			bool		vaoUsed;

			ShaderData(std::string const vs, std::string const fs, std::string const gs = "") {
				vaoUsed = false;
				if (gs.empty()) {
					shader = new Shader(vs, fs);
				}
				else {
					shader = new Shader(vs, fs, gs);
				}
			}
			~ShaderData() {
				if (vaoUsed) {
					glDeleteVertexArrays(1, &vao);
					glDeleteBuffers(1, &vbo);
				}
				delete shader;
			}
		};
		static std::unique_ptr<ShaderData>	_shaderData;

		virtual void	_draw(glm::mat4 &view, glm::vec3 &pos) = 0;
		bool			_createChunkFromFile();
		virtual void	_createChunk();

		ChunkData		_data;  // all chunk data
		wordIVec3		_chunkPos;  // position of the chunk
		std::string		_filename;
		bool			_isModifiedFromBegining;  // true if the chunk was modified
		TextureManager const &_textureManager;
};

AChunk * instanciateNewChunk(TextureManager const &textureManager);

#endif  // ACHUNK_HPP_
