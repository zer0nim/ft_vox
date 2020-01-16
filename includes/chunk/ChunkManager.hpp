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
		void	draw(Camera *cam, float nightProgress);  // draw all chunks
		void	saveAndQuit();  // save all chunks (if needed) and destroy them
		void	destroyBlock();  // destroy block using raycasting
		void	putBlock(uint8_t type);  // put block using raycasting
		void	updateBlock(wordIVec3 pos, uint8_t value) const;
		void	updateBlock(wordFVec3 pos, uint8_t value) const;
		void	updateRaycast();  // update raycast

		tWinUser								*getWinU();
		tWinUser								*getWinU() const;
		std::map<wordIVec3, AChunk*>			&getChunkMap();
		std::map<wordIVec3, AChunk*> const		&getChunkMap() const;
		wordIVec3 const							&getChunkActPos() const;
		TextureManager const					&getTextureManager() const;
		glm::mat4 const							&getProjection() const;
		uint32_t								getNbChunkLoaded() const;
		uint32_t								getNbChunkRendered() const;
		uint32_t								getNbSquareRendered() const;
		uint8_t									getBlock(wordIVec3 pos) const;
		uint8_t									getBlock(wordFVec3 pos) const;
		bool									isChunkExist(wordIVec3 const &chunkPos) const;

		std::deque<wordFVec3>				toDelete;  // list of chunks to delete
		struct Raycast {
			bool		isBlockSelected;
			wordIVec3	selectedBlock;
			uint8_t		blockType;

			bool		isBeforeBlock;
			wordIVec3	beforeSelectedBlock;

			Raycast() : isBlockSelected(false), blockType(0), isBeforeBlock(false) {}
		};
		Raycast			raycast;

	private:
		ChunkManager();
		void	_updateChunkPos(wordFVec3 const &pos);  // update the chunk pos (call each frame)
		void	_updateChunkPos(wordIVec3 const &pos);  // update the chunk pos (call each frame)
		bool	_isInChunkLoaded(wordIVec3 const &chunkPos) const;  // return true if the chunk is loaded
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
		uint32_t						_nbChunkLoaded;  // number of chunks loaded in memory
		uint32_t						_nbChunkRendered;  // number of chunks rendered on screen
		uint32_t						_nbSquareRendrered;  // number of square rendered

		std::chrono::milliseconds		_lastDestroyed;  // last ts for destroy a block
		std::chrono::milliseconds		_lastPut;  // last ts for put a block

		Shader							*_borderShader;
		uint32_t						_borderShaderVAO;
		uint32_t						_borderShaderVBO;
		static const float				_borderVertices[];
};
