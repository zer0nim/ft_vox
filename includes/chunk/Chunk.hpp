#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include "AChunk.hpp"

class Chunk : public AChunk {
	public:
		Chunk();
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		virtual void	update();
		virtual void	draw();
		virtual void	oldCreateChunk();  // TODO(zer0nim): remove
	protected:
		virtual void	_createChunk();

	private:
		static const float	_cubeData[];
};

#endif  // CHUNK_HPP_
