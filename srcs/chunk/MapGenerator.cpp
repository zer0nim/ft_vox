#include <glm/gtc/noise.hpp>
#include "MapGenerator.hpp"
#include "TextureManager.hpp"

static MapInfo mapInfo = MapInfo();

void		setSeed(uint32_t seed) {
	mapInfo.perlin.reseed(seed);
}

uint8_t		getBlock(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	float	x = (chunkPos.x + ix) * mapInfo.xFactor + 0.5;
	float	y = (chunkPos.y + iy) * mapInfo.yFactor;
	float	z = (chunkPos.z + iz) * mapInfo.zFactor + 0.5;
	uint8_t	result = 0;

	// create normal montains
	float	elevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
		+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
		+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
	elevation = std::pow(elevation, MAP_HEIGHT_EXP);
	elevation = elevation / MAP_HEIGHT_DIV + MAP_START_HEIGHT;
	if (y <= elevation) {
		if (y + mapInfo.yFactor * 1 >= elevation)
			result = TextureManager::blocksNames["grass"];
		else if (y + mapInfo.yFactor * 2 >= elevation)
			result = TextureManager::blocksNames["dirt"];
		else
			result = TextureManager::blocksNames["stone"];
	}

	// create cavern
	float	cavern;
	cavern = 1 * PERLIN(MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1)
	+  0.5 * PERLIN(MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2)
	+ 0.25 * PERLIN(MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4);
	cavern = std::pow(cavern, MAP_HEIGHT_EXP);
	if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
		// there is cavern
		float cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.5 + MAP_CAVERN_BASE_Y;
		float cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
		if (y > cavernY1 && y < cavernY2) {
			result = 0;
		}
	}

	// always a block at the first layer
	if (chunkPos.y + iy == 0) {
		result = TextureManager::blocksNames["sand"];  // replace by bedrock
	}
	return result;
}
