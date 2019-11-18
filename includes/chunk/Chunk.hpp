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

		virtual void	update();
		virtual void	draw(glm::mat4 &view);
		virtual void	oldCreateChunk();  // TODO(zer0nim): remove
		virtual void	setProjection(glm::mat4 &projection);
	protected:
		virtual void	_createChunk();

	private:
		void	sendCubeData();

		static const float	_cubeData[];
		Shader		*_naiveShader = nullptr;
		u_int32_t	_cubeVbo;
		u_int32_t	_cubeVao;
};

#endif  // CHUNK_HPP_
