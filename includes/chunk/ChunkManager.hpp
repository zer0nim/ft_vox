#pragma once

#include <map>
#include <deque>
#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Camera.hpp"

std::string	vecToString(const wordIVec3 &vec);  // vec to string -> "x_y_z"
wordIVec3	stringToVec(const std::string &s);  // string "x_y_z" to vec

class ChunkManager {
	public:
		ChunkManager(TextureManager const &textureManager, tWinUser *winU);
		explicit ChunkManager(ChunkManager const &src);
		virtual ~ChunkManager();

		ChunkManager &operator=(ChunkManager const &rhs);

		void	init(wordFVec3 camPos, glm::mat4 &projection);  // load the firsts chunks
		void	update(wordFVec3 &camPos, bool createAll = false);  // global update (call each frame)
		void	draw(glm::mat4 view, Camera *cam);  // draw all chunks
		void	saveAndQuit();  // save all chunks (if needed) and destroy them

		tWinUser								*getWinU();
		tWinUser								*getWinU() const;
		std::map<std::string, AChunk*>			&getChunkMap();
		std::map<std::string, AChunk*> const	&getChunkMap() const;
		wordIVec3 const							&getChunkActPos() const;
		TextureManager const					&getTextureManager() const;
		glm::mat4 const							&getProjection() const;
		uint32_t								getNbChunkLoaded() const;
		uint32_t								getNbChunkRendered() const;

		std::vector<std::string>				toDelete;  // list of chunks to delete

	private:
		ChunkManager();
		void	_updateChunkPos(wordFVec3 const &pos);  // update the chunk pos (call each frame)
		void	_updateChunkPos(wordIVec3 const &pos);  // update the chunk pos (call each frame)
		void	_insertChunk(wordIVec3 chunkPos, AChunk * newChunk);
		bool	_isInChunkLoaded(wordIVec3 const &chunkPos) const;  // return true if the chunk is loaded
		bool	_isChunkExist(std::string const &chunkPos) const;  // return true if the chunk exist
		bool	_isChunkExist(wordIVec3 const &chunkPos) const;

		tWinUser			*_winU;
		/*
		key: string -> "x_y_z" of the chunk (real pos: 0, 16, 32, ...)
		the value is the chunk
		*/
		std::map<std::string, AChunk*>	_chunkMap;  // map with all chunks
		wordIVec3						_chunkActPos;  // actual chunk position
		TextureManager const			&_textureManager;
		glm::mat4						_projection;
		std::deque<wordIVec3>			_toCreate;  // list of chunks to create
		uint32_t						_nbChunkLoaded;  // number of chunks loaded in memory
		uint32_t						_nbChunkRendered;  // number of chunks rendered on screen
};
