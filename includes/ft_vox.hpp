#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"
#include "utils/Camera.hpp"

#include "utils/debug.hpp"
#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#define CHUNK_SZ_X 16
#define CHUNK_SZ_Y 16
#define CHUNK_SZ_Z 16

typedef glm::tvec3<uint8_t> chunkVec3;

typedef struct	sWinUser {
	Camera		*cam;
	float		dtTime;
	float		lastFrame;
	float		width;
	float		height;
}				tWinUser;

bool	initWindow(GLFWwindow **window, const char *name, tWinUser *winU);
void	processInput(GLFWwindow *window);

#endif  // FT_VOX_HPP_
