#ifndef GREEDYCHUNK3_HPP_
#define GREEDYCHUNK3_HPP_

#include <memory>
#include <vector>
#include <array>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

#define UNKNOW_BLOCK 255

class GreedyChunk3 : public AChunk {
	public:
		explicit GreedyChunk3(TextureManager const &textureManager, ChunkManager &chunkManager);
		GreedyChunk3(GreedyChunk3 const &src);
		virtual ~GreedyChunk3();

		GreedyChunk3 &operator=(GreedyChunk3 const &rhs);

		static void			initShader(glm::mat4 &projection, TextureManager const &textureManager);
		virtual void		update(bool firstUpdate = false);
		virtual uint32_t	getNbSquareRendered() const;

	protected:
		virtual void	_draw(glm::mat4 &view, glm::vec3 &pos);

	private:
		void	sendMeshData();
		static void	sendConstUniforms(TextureManager const &textureManager);
		void	calcGreedyChunk(bool firstUpdate);
		uint8_t	getBlockOutside(wordIVec3 voxPos, std::array<AChunk*, 2> &nearbyChunks, int d, int sz);

		bool					_meshUpdated;
		bool					_needInitVao;
		u_int32_t				_vbo;
		u_int32_t				_vao;
		std::vector<float>		_faces;
		int						_nbVertices;
};

#endif  // GREEDYCHUNK3_HPP_
