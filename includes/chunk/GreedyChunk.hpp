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
		explicit GreedyChunk(TextureManager const &textureManager);
		GreedyChunk(GreedyChunk const &src);
		virtual ~GreedyChunk();

		GreedyChunk &operator=(GreedyChunk const &rhs);

		static void	initShader(glm::mat4 &projection, TextureManager const &textureManager);
		virtual void	update();

	protected:
		virtual void	_draw(glm::mat4 &view);

	private:
		struct MeshData {
			chunkVec3	pos;
			chunkVec3	size;
			uint8_t		blockId;
		};

		static void	sendCubeData(TextureManager const &textureManager);
		void	searchQuadMesh(chunkVec3 const &startI);

		std::vector<MeshData>	_meshDatas;

		static const float	_cubeData[];
		static std::array< std::array< std::array<bool, CHUNK_SZ_Z>, CHUNK_SZ_Y >, \
			CHUNK_SZ_X >	_processedVox;
};

#endif  // GREEDYCHUNK_HPP_
