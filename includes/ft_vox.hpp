#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"
#include "utils/Camera.hpp"

#include "utils/debug.hpp"
#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#define SETTINGS_FILE "assets/settings.json"

#define MAX_MAP_SIZE_X 2^16  // [bloc] type: int
#define MAX_MAP_SIZE_Y 64  // [bloc] type: int
#define MAX_MAP_SIZE_Z MAX_MAP_SIZE_X  // [bloc] type: int

#define CHUNK_SZ_X 16  // [bloc] type: int
#define CHUNK_SZ_Y 64  // [bloc] type: int
#define CHUNK_SZ_Z 16  // [bloc] type: int

#define MAX_Y_CHUNK				MAX_MAP_SIZE_Y / CHUNK_SZ_Y  // [chunk] type: int

class Chunk;
class GreedyChunk;
#define CHUNK_OBJECT			GreedyChunk2  // the chunk object used

/*
generation type
*/
#define GENERATION_VOID		0
#define GENERATION_NORMAL	1

/*
filesystem
*/
#define CHUNK_EXTENSION ".chunk"

typedef glm::tvec3<int8_t>	chunkVec3;  // used for chunk coordinate
typedef glm::tvec3<int32_t>	wordIVec3;  // used for word coordinate in int (bloc)
typedef glm::tvec3<float>	wordFVec3;  // user for word coordinate in float (camera)

enum class Direction {
	FRONT,
	RIGHT,
	BACK,
	LEFT,
	TOP,
	BOTTOM
};

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
std::chrono::milliseconds getMs();
bool	usage();
class TextRender;
class ChunkManager;
bool	argparse(int nbArgs, char const **args);
void	drawText(GLFWwindow *window, TextRender &textRender, int actFps, ChunkManager &chunkManager);
bool	createDir(std::string const &dirNames);
bool	createDir(char const *dirNames);
bool	createMapFiles();
void	setDefaultSettings();
void	loadSettings(std::string settingFile);
bool	saveMap(Camera &cam);

struct Settings {
	struct Global {
		int32_t	renderDist;
		struct Files {
			std::string	mapsPath;
			std::string	chunkPath;
			std::string mapSettingsPath;
			bool		saveAllChunks;
		};
		Files	files;
		struct Screen {
			uint32_t	width;
			uint32_t	height;
			uint32_t	fps;
			struct Text {
				std::string	path;
				uint32_t	size;
			};
			std::map<std::string, Text> text;
		};
		Screen	screen;
	};
	Global	g;  // global
	struct Map {
		std::string	mapName;  // don't set it in settings.json
		std::string fullMapName;  // don't set it in settings.json
		uint32_t	seed;
		uint32_t	generationType;
		struct CameraStartPos {
			glm::vec3	pos;
			float		yaw;
			float		pitch;
		};
		CameraStartPos	cameraStartPos;
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
};

extern Settings s;

#endif  // FT_VOX_HPP_
