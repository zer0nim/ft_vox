#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"
#include "utils/Camera.hpp"

#include "utils/debug.hpp"
#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#define MAX_MAP_SIZE_X 2^16  // [bloc] type: int
#define MAX_MAP_SIZE_Y 64  // [bloc] type: int
#define MAX_MAP_SIZE_Z MAX_MAP_SIZE_X  // [bloc] type: int

#define CHUNK_SZ_X 16  // [bloc] type: int
#define CHUNK_SZ_Y 64  // [bloc] type: int
#define CHUNK_SZ_Z 16  // [bloc] type: int

#define RENDER_DISTANCE_CHUNK	1  // [chunk] type: int
#define MAX_Y_CHUNK				MAX_MAP_SIZE_Y / CHUNK_SZ_Y  // [chunk] type: int

class Chunk;
class GreedyChunk;
#define CHUNK_OBJECT			GreedyChunk2  // the chunk object used

/*
filesystem
*/
#define SAVE_ALL_CHUNKS false  // type: bool -> save all chunk in files (not only modified)
#define MAPS_PATH "/tmp/ft_vox/maps/"
#define CHUNK_PATH "chunks/"
#define CHUNK_EXTENSION ".chunk"
#define SEED_FILE "seed"

/*
generation type
*/
#define GENERATION_VOID		0
#define GENERATION_NORMAL	1
#define GENERATION_TYPE GENERATION_NORMAL  // choose the generation type

typedef glm::tvec3<int8_t>	chunkVec3;   // used for chunk coordinate
typedef glm::tvec3<int32_t>	wordIVec3;  // used for word coordinate in int (bloc)
typedef glm::tvec3<float>	wordFVec3;    // user for word coordinate in float (camera)

typedef struct	sWinUser {
	Camera		*cam;
	float		dtTime;
	float		lastFrame;
	float		width;
	float		height;

	bool		showInfo;  // show info module (F3)
	bool		showHelp;  // show help module (F3 + H)
	bool		freezeChunkUpdate;  // freeze chunk update (F3 + F)
	int8_t		polygonRenderMode;  // toggle polygon render mode (F3 + P)
}				tWinUser;

struct Locker {
	bool	ask;
	bool	isLocked;

	/*
	thread locker:
	main thread ask other thread to lock:
		while loop:
			locker.ask = true;
			if (locker.isLocked):
				-> do some stuff here: other thread is locked
				locker.ask = false;
	other thread
		while loop:
			locker.isLocked = locker.ask;
			if (locker.isLocked == false)
				-> do some stuff here: thread unlocked
	*/
	Locker() : ask(false), isLocked(false) {}
};

class ChunkManager;
struct ThreadupdateArgs {
	GLFWwindow		*window;
	ChunkManager	&chunkManager;
	wordFVec3		&camPos;
	bool			quit;
	Locker			deleteLocker;  // lock thread to delete chunks safely

	ThreadupdateArgs(GLFWwindow *window_, ChunkManager &chunkManager_, wordFVec3 &camPos_) :
	window(window_), chunkManager(chunkManager_), camPos(camPos_), quit(false) {}
};

bool	initWindow(GLFWwindow **window, const char *name, tWinUser *winU);
void	processInput(GLFWwindow *window);

#endif  // FT_VOX_HPP_
