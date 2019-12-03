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
		explicit GreedyChunk2(TextureManager const &textureManager);
		GreedyChunk2(GreedyChunk2 const &src);
		virtual ~GreedyChunk2();

		GreedyChunk2 &operator=(GreedyChunk2 const &rhs);

		static void	initShader(glm::mat4 &projection, TextureManager const &textureManager);
		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view);

	private:
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
		static void	sendConstUniforms(TextureManager const &textureManager);
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
};

#endif  // GREEDYCHUNK2_HPP_
