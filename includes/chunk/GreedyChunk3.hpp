#ifndef GREEDYCHUNK3_HPP_
#define GREEDYCHUNK3_HPP_

#include <memory>
#include <vector>
#include <array>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

class GreedyChunk3 : public AChunk {
	public:
		explicit GreedyChunk3(TextureManager const &textureManager, glm::mat4 &projection);
		GreedyChunk3(GreedyChunk3 const &src);
		virtual ~GreedyChunk3();

		GreedyChunk3 &operator=(GreedyChunk3 const &rhs);

		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view);

	private:
		struct ShaderData {
			Shader		*greedyShader;

			ShaderData() {
				greedyShader = new Shader("shaders/greedyChunk3_vs.glsl", \
				"shaders/naive_fs.glsl", "shaders/greedyChunk3_gs.glsl");
			}
			~ShaderData() {
				delete greedyShader;
			}
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

		void	sendMeshData();
		void	sendConstUniforms();
		VoxFace	*getVoxFace(chunkVec3 const pos, Direction const side);
		void	calcGreedyChunk();
		void	fillVectLine(std::vector<float> &vertices, int & i, \
		chunkVec3 const &pos, glm::tvec2<int8_t> textUv, Quad const &q);

		bool					_meshUpdated;
		bool					_needInitVao;
		u_int32_t				_vbo;
		u_int32_t				_vao;
		std::vector<Quad>		_quads;
		int						_nbVertices;

		static const float	_cubeData[];
		static std::unique_ptr<ShaderData>	_shaderData;
};

#endif  // GREEDYCHUNK3_HPP_
