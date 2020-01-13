#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"
#include "utils/Camera.hpp"
#include "utils/Logging.hpp"

#include "utils/debug.hpp"
#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#define DEF_SETTINGS_FILE "assets/settings.json"

#define MAX_MAP_SIZE_X 16384  // [bloc] type: int
#define MAX_MAP_SIZE_Y 256    // [bloc] type: int
#define MAX_MAP_SIZE_Z MAX_MAP_SIZE_X  // [bloc] type: int

#define CHUNK_SZ_X 16   // [bloc] type: int
#define CHUNK_SZ_Y 128  // [bloc] type: int
#define CHUNK_SZ_Z 16   // [bloc] type: int

#define TREE_RADIUS	2 	// [bloc] type: int -> max radius of a tree (if 2 -> max is 5*5)
#define TREE_HEIGHT	8 	// [bloc] type: int -> max height of a tree

#define MAX_Y_CHUNK				MAX_MAP_SIZE_Y / CHUNK_SZ_Y  // [chunk] type: int

#define ENABLE_MAX_CREATED_CHUNK_UPDATE	true  // enable / disable option
#define LOAD_ALL_BEFORE_OPEN_WINDOW		false
#define MAX_CREATED_CHUNK_UPDATE_COUNT	2  // create max # chunks each update call (don't create all at the same update)
#define REMOVE_CHUNKS_BORDERS			true
#define MAX_RENDER_CHUNK_UPDATE_COUNT	1  // update render of # chunks (if there are all created)

#define NB_UPDATE_THREADS				4

#define SMART_LOAD_ORDER				true  // load chunks next to the player first

/*
Chunk
*/
#ifndef CHUNK  // make DEFINE="CHUNK=Chunk"
	#define CHUNK		Chunk
#endif
#define CHUNK_OBJECT	CHUNK  // the chunk object used


/*
generation type
*/
#define GENERATION_VOID				0
#define GENERATION_FLAT_MAP			1
#define GENERATION_NORMAL			2

/*
gamemode
*/
#define GAMEMODE_SURVIVAL			0
#define GAMEMODE_CREATIVE			1

/*
filesystem
*/
#define CHUNK_EXTENSION ".chunk"

typedef glm::tvec3<uint8_t>	chunkVec3;  // used for chunk coordinate
typedef glm::tvec3<int32_t>	wordIVec3;  // used for word coordinate in int (bloc)
typedef CAMERA_VEC3			wordFVec3;  // user for word coordinate in float (camera)

enum class Direction {
	FRONT,
	RIGHT,
	BACK,
	LEFT,
	TOP,
	BOTTOM
};

class ChunkManager;
typedef struct		sWinUser {
	Camera			*cam;
	Camera			*camCrea;
	Camera			*camSurv;
	ChunkManager	*chunkManager;
	float			dtTime;
	float			lastFrame;

	bool			showInfo;  // show info module (F3)
	bool			showHelp;  // show help module (F3 + H)
	bool			showObjList;  // show objects list (F3 + L)
	bool			showCommands;  // show commands list (F3 + C)
	bool			freezeChunkUpdate;  // freeze chunk update (F3 + F)
	bool			putBlock;  // mouse left clicked
	bool			destroyBlock;  // mouse right clicked
	int8_t			polygonRenderMode;  // toggle polygon render mode (F3 + P)
}					tWinUser;

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
	uint8_t			threadID;
	GLFWwindow		*window;
	ChunkManager	&chunkManager;
	bool			quit;
	Locker			deleteLocker;  // lock thread to delete chunks safely

	ThreadupdateArgs(uint8_t id, GLFWwindow *window_, ChunkManager &chunkManager_) :
	threadID(id), window(window_), chunkManager(chunkManager_), quit(false) {}
};

bool	initWindow(GLFWwindow **window, const char *name, tWinUser *winU);
void	processInput(GLFWwindow *window);
std::chrono::milliseconds getMs();
bool	usage();
class TextRender;
class ChunkManager;
bool	argparse(int nbArgs, char const **args, bool isBeforeSettings = false);
void	drawText(GLFWwindow *window, TextRender &textRender, int actFps, ChunkManager &chunkManager);
bool	createDir(std::string const &dirNames);
bool	createDir(char const *dirNames);
bool	createMapFiles();
void	setDefaultSettings();
void	loadSettings(std::string settingFile);
bool	saveMap(Camera *cam);

struct Settings {
	struct Global {
		int32_t	renderDist;
		struct Files {
			std::string settingsFile;
			std::string	mapsPath;
			std::string	chunkPath;
			std::string mapSettingsPath;
			bool		saveAllChunks;
		};
		Files	files;
		struct Screen {
			bool		fullscreen;
			uint32_t	width;
			uint32_t	height;
			uint32_t	fps;
			bool		enableTransparency;
			struct Text {
				std::string	path;
				uint32_t	size;
			};
			std::map<std::string, Text> text;
		};
		Screen		screen;
		struct Player {
			struct Survival {
				float	movementSpeed;
				float	runFactor;
				float	gravity;
				float	jumpHeight;
				float	jumpSpeed;
				float	height;
				float	eyeHeight;
				float	radius;
			};
			Survival	survival;
			struct Creative {
				float	movementSpeed;
				float	runFactor;
			};
			Creative	creative;
			float		mouseSensitivity;
			uint32_t	delayPutMs;  // delay between put 2 blocks
			uint32_t	delayDestroyMs;  // delay between destroy 2 blocks
		};
		Player		player;

		struct Fog {
			bool		enabled;
			uint32_t	width;
			glm::vec4	color;
		};
		Fog		fog;
	};
	Global	g;  // global
	struct Map {
		std::string	mapName;  // don't set it in settings.json
		std::string fullMapName;  // don't set it in settings.json
		uint32_t	seed;
		uint32_t	generationType;
		bool		generateTree;
		bool		generateOre;
		struct CameraStartPos {
			glm::vec3	pos;
			float		yaw;
			float		pitch;
		};
		CameraStartPos	cameraStartPos;

		struct FlatMap {
			uint32_t	fromY;  // included
			uint32_t	toY;    // included
			std::string	blockName;
		};
		std::vector<FlatMap>	flatMap;
		uint8_t					handBlockID;
		uint8_t					gamemode;
	};
	Map m;  // map

	class SettingsError : public std::exception {
		public:
			virtual const char* what() const throw() = 0;
	};
	class FailedToOpenException : public SettingsError {
		public:
			explicit FailedToOpenException(std::string const &msg) : _msg(msg) {
				std::string s("Failed to open " + _msg);
				s.resize(511);  // limit string size
				s.copy(cstr, s.size() + 1);
				cstr[s.size()] = '\0';
			}
			virtual const char * what() const throw() {
				return cstr;
			}
		private:
			std::string const _msg;
			char	cstr[512];
	};
	class JsonParseException : public SettingsError {
		public:
			virtual const char* what() const throw() {
				return ("Failed to parse json texture settings");
			}
	};

	/* global variables */
	std::mutex	mutexChunkMap;
	std::mutex	mutexToCreate;
	std::mutex	mutexToDelete;
	std::mutex	mutexCamera;
	std::mutex	mutexOthers;
};

extern Settings s;

#endif  // FT_VOX_HPP_
