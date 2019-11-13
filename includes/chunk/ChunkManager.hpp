#pragma once

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

class ChunkManager {
	public:
		ChunkManager();
		ChunkManager(ChunkManager const &src);
		virtual ~ChunkManager();

		ChunkManager &operator=(ChunkManager const &rhs);
	protected:
	private:
};
