#ifndef GREEDYCHUNK2_HPP_
#define GREEDYCHUNK2_HPP_

#include <memory>
#include <vector>
#include <array>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

class GreedyChunk2 : public AChunk {
	public:
		explicit GreedyChunk2(TextureManager const &textureManager, glm::mat4 &projection);
		GreedyChunk2(GreedyChunk2 const &src);
		virtual ~GreedyChunk2();

		GreedyChunk2 &operator=(GreedyChunk2 const &rhs);

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
				naiveShader = new Shader("shaders/greedyChunk2_vs.glsl", "shaders/naive_fs.glsl");
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

		enum class Direction {
			SOUTH,
			NORTH,
			EAST,
			WEST,
			TOP,
			BOTTOM
		};

		struct VoxFace {
			uint8_t		type;
			bool		transparent;  // for face culling
			Direction	side;

			bool operator==(VoxFace const &rhs) const {
				return (this->transparent == rhs.transparent && this->type == rhs.type);
			};
			bool operator!=(VoxFace const &rhs) const {
				return !(*this == rhs);
			};
		};

		struct Quad {
			VoxFace		voxFace;
			uint8_t		width;
			uint8_t		height;
			bool		backFace;
			chunkVec3	bottomLeft;
			chunkVec3	topLeft;
			chunkVec3	topRight;
			chunkVec3	bottomRight;
		};
		std::vector<Quad>	_quads;

		VoxFace	*getVoxFace(chunkVec3 const pos, Direction const side);
		void	calcGreedyChunk();

		std::vector<MeshData>	_meshDatas;

		static const float	_cubeData[];
		static std::unique_ptr<ShaderData>	_shaderData;
};

#endif  // GREEDYCHUNK2_HPP_
