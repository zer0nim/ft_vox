#ifndef GREEDYCHUNK_HPP_
#define GREEDYCHUNK_HPP_

#include <memory>
#include <vector>
#include <array>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

class GreedyChunk : public AChunk {
	public:
		explicit GreedyChunk(TextureManager const &textureManager, glm::mat4 &projection);
		GreedyChunk(GreedyChunk const &src);
		virtual ~GreedyChunk();

		GreedyChunk &operator=(GreedyChunk const &rhs);

		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view);

	private:
		void	sendCubeData();

		struct ShaderData {
			Shader		*naiveShader;
			u_int32_t	cubeVbo;
			u_int32_t	cubeVao;

			ShaderData() {
				naiveShader = new Shader("shaders/greedyChunk_vs.glsl", "shaders/naive_fs.glsl");
				cubeVbo = 0;
				cubeVao = 0;
			}

			~ShaderData() {
				delete naiveShader;
			}
		};

		struct MeshData {
			chunkVec3	pos;
			chunkVec3	size;
			uint8_t		blockId;
		};

		void	searchQuadMesh(chunkVec3 const &startI);

		std::vector<MeshData>	_meshDatas;

		static const float	_cubeData[];
		static std::unique_ptr<ShaderData>	_shaderData;
		static std::array< std::array< std::array<bool, CHUNK_SZ_Z>, CHUNK_SZ_Y >, \
			CHUNK_SZ_X >	_processedVox;
};

#endif  // GREEDYCHUNK_HPP_
