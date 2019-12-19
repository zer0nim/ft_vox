#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

class Chunk : public AChunk {
	public:
		explicit Chunk(TextureManager const &textureManager);
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		static void	initShader(glm::mat4 &projection, TextureManager const &textureManager);
		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view, glm::vec3 &pos);

	private:
		static void	sendCubeData(TextureManager const &textureManager);

		static const float	_cubeData[];
};

#endif  // CHUNK_HPP_
