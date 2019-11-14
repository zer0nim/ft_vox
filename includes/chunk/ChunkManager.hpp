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

std::string	vecToString(const wordIVec3 &vec);  // vec to string -> "x_y_z"
wordIVec3	stringToVec(const std::string &s);  // string "x_y_z" to vec

class ChunkManager {
	public:
		explicit ChunkManager(std::string const &mapName);
		explicit ChunkManager(ChunkManager const &src);
		virtual ~ChunkManager();

		ChunkManager &operator=(ChunkManager const &rhs);

		void	init(wordFVec3 camPos);  // load the firsts chunks
		void	update(wordFVec3 camPos);  // global update (call each frame)

		std::string const &getMapName() const;
		std::map<std::string, AChunk*>			&getChunkMap();
		std::map<std::string, AChunk*> const	&getChunkMap() const;

	private:
		ChunkManager();
		void	_updateChunkPos(wordFVec3 const &pos);  // update the chunk pos (call each frame)
		void	_updateChunkPos(wordIVec3 const &pos);  // update the chunk pos (call each frame)
		void	_insertChunk(wordIVec3 chunkPos, AChunk * newChunk);
		bool	_isInChunkLoaded(wordIVec3 const &chunkPos) const;  // return true if the chunk is loaded
		bool	_isChunkExist(std::string const &chunkPos) const;  // return true if the chunk exist
		bool	_isChunkExist(wordIVec3 const &chunkPos) const;

		std::string const	&_mapName;
		/*
		key: string -> "x_y_z" of the chunk (real pos: 0, 16, 32, ...)
		the value is the chunk
		*/
		std::map<std::string, AChunk*>	_chunkMap;  // map with all chunks
		wordIVec3						_chunkActPos;  // actual chunk position
};
