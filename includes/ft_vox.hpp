#ifndef FT_VOX_HPP_
#define FT_VOX_HPP_

#include "commonInclude.hpp"

#define CHUNK_SZ_X 32  // [bloc] type: int
#define CHUNK_SZ_Y 128  // [bloc] type: int
#define CHUNK_SZ_Z 16  // [bloc] type: int

#define RENDER_DISTANCE_CHUNK	2  // [chunk] type: int
#define MAX_Y_CHUNK				256 / CHUNK_SZ_Y  // [chunk] type: int

typedef glm::tvec3<int8_t>	chunkVec3;   // used for chunk coordinate
typedef glm::tvec3<int32_t>	wordIVec3;  // used for word coordinate in int (bloc)
typedef glm::tvec3<float>	wordFVec3;    // user for word coordinate in float (camera)

#endif  // FT_VOX_HPP_
