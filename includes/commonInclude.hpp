#ifndef COMMONINCLUDE_HPP_
#define COMMONINCLUDE_HPP_

#ifndef DEBUG
	#define DEBUG						false  // type: bool -> enable / disable the debug mode
#endif
#define DEBUG_FPS_LOW				DEBUG & false
#ifndef SHOWFPS
	#define DEBUG_SHOW_FPS				DEBUG & false  // show all fps count
#else
	#undef DEBUG
	#define DEBUG					true
	#define DEBUG_SHOW_FPS				true  // show all fps count
#endif
#define DEBUG_SHOW_FPS_ALLTHREAD	DEBUG & false  // show all fps count for all threads


// glfw
#include <glad/glad.h>
// #define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"  // to print vect/mat with glm::to_string

namespace glm {
  template <typename T>
  bool operator<(tvec3<T> const &lhs, tvec3<T> const &rhs) {
    return (lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y)
	|| (lhs.x < rhs.x && lhs.y < rhs.y)
	|| (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z));
  }
};

#endif  // COMMONINCLUDE_HPP_
