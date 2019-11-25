#include <glm/gtc/noise.hpp>
#include "MapGenerator.hpp"
#include "TextureManager.hpp"

static MapInfo mapInfo = MapInfo();

void		setSeed(uint32_t seed) {
	mapInfo.perlin.reseed(seed);
}

uint8_t		getBlockVoid(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	(void)chunkPos;
	(void)ix;
	(void)iy;
	(void)iz;
	return 0;
}

uint8_t		getBlockNormal(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	float	x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
	float	y = (chunkPos.y + iy) * mapInfo.yFactor;
	float	z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;
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
	float	cavernY = y * MAP_CAVERN_Y_MULTIPLIER;
	cavern = 1 * PERLIN(MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1)
	+  0.5 * PERLIN(MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2)
	+ 0.25 * PERLIN(MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4);
	cavern = std::pow(cavern, MAP_HEIGHT_EXP);
	if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
		// there is cavern
		float cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.5 + MAP_CAVERN_BASE_Y;
		float cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
		if (cavernY > cavernY1 && cavernY < cavernY2) {
			result = 0;
		}
	}

	// always a block at the first layer
	if (chunkPos.y + iy < MAP_MAX_BEDROCK_HEIGHT) {
		float	elevation = PERLIN(10 * x, 10 * z);
		elevation = std::pow(elevation, 0.4);
		elevation *= MAP_MAX_BEDROCK_HEIGHT;
		if (chunkPos.y + iy == 0 || chunkPos.y + iy <= static_cast<uint8_t>(elevation))
			result = TextureManager::blocksNames["bedrock"];  // replace by bedrock
	}
	return result;
}

uint8_t		getBlockFlatMap(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	(void)ix;
	(void)iz;
	uint32_t y = chunkPos.y + iy;
	for (auto it = s.m.flatMap.begin(); it != s.m.flatMap.end(); it++) {
		if (it->fromY <= y && y <= it->toY) {
			return TextureManager::blocksNames[it->blockName];
		}
	}
	return 0;
}

uint8_t		getBlock(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	if (s.m.generationType == GENERATION_VOID)
		return getBlockVoid(chunkPos, ix, iy, iz);
	if (s.m.generationType == GENERATION_NORMAL)
		return getBlockNormal(chunkPos, ix, iy, iz);
	else
		return getBlockFlatMap(chunkPos, ix, iy, iz);
}
