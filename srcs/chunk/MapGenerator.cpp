#include <glm/gtc/noise.hpp>
#include "MapGenerator.hpp"

static MapInfo mapInfo = MapInfo();

uint8_t		getBlock(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	float	x = (chunkPos.x + ix) * mapInfo.xFactor + 0.5;
	float	y = (chunkPos.y + iy) * mapInfo.yFactor;
	float	z = (chunkPos.z + iz) * mapInfo.zFactor + 0.5;
	uint8_t	result = 0;

	// create normal montains
	float	elevation;
	#if MAP_METHOD == 0
		elevation = 1 * glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1))
			+  0.5 * glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2))
			+ 0.25 * glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4));
	#else
		float e0 =    1 * 2 * (0.5 - abs(0.5 - glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * 1 * x,
			MAP_FREQ_MONTAIN * 1 * z))));
		float e1 =  0.5 * 2 * (0.5 - abs(0.5 - glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * 2 * x,
			MAP_FREQ_MONTAIN * 2 * z)))) * e0;
		float e2 = 0.25 * 2 * (0.5 - abs(0.5 - glm::perlin(glm::vec2(MAP_FREQ_MONTAIN * 4 * x,
			MAP_FREQ_MONTAIN * 4 * z)))) * (e0 + e1);
		elevation = e0 + e1 + e2;
	#endif
	elevation = std::pow(elevation, MAP_HEIGHT_EXP);
	elevation = elevation / MAP_HEIGHT_DIV + MAP_START_HEIGHT;
	if (y <= elevation) {
		if (y + mapInfo.yFactor * 1 >= elevation)
			result = 4;
		else if (y + mapInfo.yFactor * 3 >= elevation)
			result = 1;
		else
			result = 3;
	}

	// create cavern
	float	cavern;
	cavern = glm::perlin(glm::vec3(x * MAP_CAVERN_FREQ, y * 30, z * MAP_CAVERN_FREQ));
	if (cavern > 0.2) {
		// result = 0;
	}

	if (chunkPos.y + iy == 0) {
		result = 2;  // always a block at the first layer
	}
	return result;
}
