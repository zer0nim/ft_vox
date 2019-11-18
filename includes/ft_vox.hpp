#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"
#include "utils/Camera.hpp"

#include "utils/debug.hpp"
#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#define CHUNK_SZ_X 16  // [bloc] type: int
#define CHUNK_SZ_Y 16  // [bloc] type: int
#define CHUNK_SZ_Z 16  // [bloc] type: int

#define RENDER_DISTANCE_CHUNK	2  // [chunk] type: int
#define MAX_Y_CHUNK				256 / CHUNK_SZ_Y  // [chunk] type: int

/*
filesystem
*/
#define SAVE_ALL_CHUNKS false  // type: bool -> save all chunk in files (not only modified)
#define MAPS_PATH "/tmp/ft_vox/maps/"
#define CHUNK_PATH "chunks/"
#define CHUNK_EXTENSION ".chunk"

typedef glm::tvec3<int8_t>	chunkVec3;   // used for chunk coordinate
typedef glm::tvec3<int32_t>	wordIVec3;  // used for word coordinate in int (bloc)
typedef glm::tvec3<float>	wordFVec3;    // user for word coordinate in float (camera)

typedef struct	sWinUser {
	Camera		*cam;
	float		dtTime;
	float		lastFrame;
	float		width;
	float		height;

	bool		showInfo;
}				tWinUser;

bool	initWindow(GLFWwindow **window, const char *name, tWinUser *winU);
void	processInput(GLFWwindow *window);

#endif  // FT_VOX_HPP_
