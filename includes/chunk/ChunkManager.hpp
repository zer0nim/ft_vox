#pragma once

#include <map>
#include "ft_vox.hpp"
#include "AChunk.hpp"

/*
stocker les chunks dans une map:
map < <int int int> <AChunk*> >

les int correspondent aux coordonee reel (0, 16, 32, ...) et pas 0, 1, 2

avantage:
- acces rapide
- destruction rapide
- acces aux voisins rapide

defaut:
- chiant de savoir si les truc a delete sont encore la (peut etre une fonction OnChunkChange ?)


TODO:
stocker le chunk actuel
appel de fonction si le chunk actuel change
fonction de creation de chunk
fonction de destruction de chunk

? load chunk from file (in AChunk) ?
*/

std::string vecToString(wordIVec3 vec);

class ChunkManager {
	public:
		ChunkManager();
		ChunkManager(ChunkManager const &src);
		virtual ~ChunkManager();

		ChunkManager &operator=(ChunkManager const &rhs);

		void	init(wordFVec3 camPos);  // load the firsts chunks
		void	update(wordFVec3 camPos);  // global update (call each frame)
	protected:
	private:
		void	_updateChunkPos(wordFVec3 pos);  // update the chunk pos (call each frame)
		void	_updateChunkPos(wordIVec3 pos);  // update the chunk pos (call each frame)
		void	_insertChunk(wordIVec3 chunkPos, AChunk * newChunk);
		/*
		key: vec3 -> pos x y & z of the chunk (real pos: 0, 16, 32, ...)

		the value is the chunk
		*/
		std::map<std::string, AChunk*>	_chunkMap;  // map with all chunks
		wordIVec3						_chunkActPos;  // actual chunk position
};
