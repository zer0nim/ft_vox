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
	if (y <= MAP_CAVERN_MAX_HEIGHT) {
		float	cavern;
		cavern = PERLIN(x * MAP_CAVERN_FREQ, y * MAP_CAVERN_FREQ, z * MAP_CAVERN_FREQ);
		if (cavern > 0.2) {
			result = 0;
		}
	}

	// always a block at the first layer
	if (chunkPos.y + iy == 0) {
		result = TextureManager::blocksNames["stone"];
	}
	return result;
}
