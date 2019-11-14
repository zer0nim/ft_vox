#ifndef CHUNK_HPP_
#define CHUNK_HPP_

#include "AChunk.hpp"

class Chunk : public AChunk {
	public:
		Chunk();
		Chunk(Chunk const &src);
		virtual ~Chunk();

		Chunk &operator=(Chunk const &rhs);

		virtual void	createChunk();
		virtual void	draw();
	protected:
	private:
		void	_update();

		static const float	_cubeData[];
};

#endif  // CHUNK_HPP_
