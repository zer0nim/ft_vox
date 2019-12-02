#ifndef COMMONINCLUDE_HPP_
#define COMMONINCLUDE_HPP_

#define DEBUG						true  // type: bool -> enable / disable the debug mode
#define DEBUG_FPS_LOW				DEBUG & false
#define DEBUG_SHOW_FPS				DEBUG & false  // show all fps count
#define DEBUG_SHOW_FPS_ALLTHREAD	DEBUG & false  // show all fps count for all threads

// glfw
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif  // COMMONINCLUDE_HPP_
