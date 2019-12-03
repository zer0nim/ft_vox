#pragma once

#include <map>
#include <deque>
#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Camera.hpp"

class ChunkManager {
	public:
		ChunkManager(TextureManager const &textureManager, tWinUser *winU);
		explicit ChunkManager(ChunkManager const &src);
		virtual ~ChunkManager();

		ChunkManager &operator=(ChunkManager const &rhs);

		void	init(wordFVec3 camPos, glm::mat4 &projection);  // load the firsts chunks
		void	update(wordFVec3 &camPos, uint8_t threadID, bool createAll = false);  // global update (call each frame)
		void	draw(glm::mat4 view, Camera *cam);  // draw all chunks
		void	saveAndQuit();  // save all chunks (if needed) and destroy them

		tWinUser								*getWinU();
		tWinUser								*getWinU() const;
		std::map<wordIVec3, AChunk*>			&getChunkMap();
		std::map<wordIVec3, AChunk*> const		&getChunkMap() const;
		wordIVec3 const							&getChunkActPos() const;
		TextureManager const					&getTextureManager() const;
		glm::mat4 const							&getProjection() const;
		uint32_t								getNbChunkLoaded() const;
		uint32_t								getNbChunkRendered() const;

		std::deque<wordFVec3>				toDelete;  // list of chunks to delete

	private:
		ChunkManager();
		void	_updateChunkPos(wordFVec3 const &pos);  // update the chunk pos (call each frame)
		void	_updateChunkPos(wordIVec3 const &pos);  // update the chunk pos (call each frame)
		bool	_isInChunkLoaded(wordIVec3 const &chunkPos) const;  // return true if the chunk is loaded
		bool	_isChunkExist(wordIVec3 const &chunkPos) const;
		uint8_t	_getID(wordIVec3 const &chunkPos) const;
		uint8_t	_getID(int32_t const x) const;

		tWinUser			*_winU;
		/*
		key: string -> "x_y_z" of the chunk (real pos: 0, 16, 32, ...)
		the value is the chunk
		*/
		std::map<wordIVec3, AChunk*>	_chunkMap;  // map with all chunks
		wordIVec3						_chunkActPos;  // actual chunk position
		std::array<wordIVec3, NB_UPDATE_THREADS>	_lastChunkPos;
		TextureManager const			&_textureManager;
		glm::mat4						_projection;
		std::array<std::deque<wordIVec3>, NB_UPDATE_THREADS>	_toCreate;  // list of chunks to create
		std::array<uint32_t, NB_UPDATE_THREADS>					_nbChunkLoaded;  // number of chunks loaded in memory
		uint32_t						_nbChunkRendered;  // number of chunks rendered on screen
};
