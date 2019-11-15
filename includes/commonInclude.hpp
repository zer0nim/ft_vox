#ifndef COMMONINCLUDE_HPP_
#define COMMONINCLUDE_HPP_

#define DEBUG false  // type: bool -> enable / disable the debug mode
#define SCREEN_W 800  // [px] type: int ->width of the screen in pixels
#define SCREEN_H 600  // [px] type: int -> height of the screen in pixels
#define FPS 30  // [Hz] type: int -> fps of the main loop
#define LOOP_TIME (1000 / FPS)  // [s] type: float -> duration in s for a loop

// glfw
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif  // COMMONINCLUDE_HPP_
