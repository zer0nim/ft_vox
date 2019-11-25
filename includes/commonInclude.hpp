#ifndef COMMONINCLUDE_HPP_
#define COMMONINCLUDE_HPP_

#define DEBUG true  // type: bool -> enable / disable the debug mode
#define DEBUG_FPS_LOW DEBUG & false
// #define SCREEN_W 1200  // [px] type: int ->width of the screen in pixels
// #define SCREEN_H 800  // [px] type: int -> height of the screen in pixels
// #define FPS 120  // [Hz] type: int -> fps of the main loop
// #define LOOP_TIME (1000 / FPS)  // [s] type: float -> duration in s for a loop

#define TEXT_SIZE_TITLE 40
#define TEXT_SIZE_NORMAL 20

// glfw
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <map>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"  // to print vect/mat with glm::to_string

#endif  // COMMONINCLUDE_HPP_
