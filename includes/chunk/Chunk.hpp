#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include <memory>
#include <vector>
#include <array>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "commonInclude.hpp"
#include "utils/Shader.hpp"

#define UNKNOW_BLOCK 255

class Chunk : public AChunk {
	public:
		explicit Chunk(TextureManager const &textureManager, ChunkManager &chunkManager);
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		static void			initShader(glm::mat4 &projection, TextureManager const &textureManager);
		virtual void		update(bool isChunkMapMutexed = true);
		virtual bool		renderUpdate(int32_t startX, int32_t startZ);  // return true if updated
		virtual uint32_t	getNbSquareRendered() const;

	protected:
		virtual void	_draw(CAMERA_MAT4 &view, wordIVec3 &chunkOffset, CAMERA_VEC3 &pos, \
		float nightProgress);

	private:
		void	sendMeshData();
		static void	sendConstUniforms(TextureManager const &textureManager);
		void	calcGreedyChunk(bool isChunkMapMutexed);
		uint8_t	getBlockOutside(wordIVec3 voxPos, std::array<AChunk*, 2> &nearbyChunks, int d, int sz);

		bool					_meshUpdated;
		bool					_needInitVao;
		u_int32_t				_vbo;
		u_int32_t				_vao;
		std::vector<float>		_faces;
		int						_nbVertices;
};

#endif  // CHUNK_HPP_
