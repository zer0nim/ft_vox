#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include <memory>

#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

class Chunk : public AChunk {
	public:
		explicit Chunk(TextureManager const &textureManager, glm::mat4 &projection);
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view) const;

	private:
		void	sendCubeData();

		struct ShaderData {
			Shader		*naiveShader;
			u_int32_t	cubeVbo;
			u_int32_t	cubeVao;

			ShaderData() {
				naiveShader = new Shader("shaders/naive_vs.glsl", "shaders/naive_fs.glsl");
				cubeVbo = 0;
				cubeVao = 0;
			}

			~ShaderData() {
				delete naiveShader;
			}
		};

		static const float	_cubeData[];
		static std::unique_ptr<ShaderData>	_shaderData;
};

#endif  // CHUNK_HPP_
