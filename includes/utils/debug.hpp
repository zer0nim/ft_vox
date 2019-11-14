#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include "commonInclude.hpp"

GLenum  checkError_(const char *file, int line);
void    checkErrorExit_(const char *file, int line);

#endif  // DEBUG_HPP_
