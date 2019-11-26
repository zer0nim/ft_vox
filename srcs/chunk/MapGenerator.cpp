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

static uint8_t	_getBlockElevation(uint32_t realY, float fy, float montainsElevation, float cavernY1, float cavernY2,
float bedrockElevation) {
	// create bedrock
	if (realY < MAP_MAX_BEDROCK_HEIGHT
	&& (realY == 0 || realY <= static_cast<uint8_t>(bedrockElevation))) {
		return TextureManager::blocksNames["bedrock"];  // replace by bedrock
	}

	// cavern
	if (cavernY1 > 0) {
		float cavernY = fy * MAP_CAVERN_Y_MULTIPLIER;
		if (cavernY > cavernY1 && cavernY < cavernY2) {
			return 0;
		}
	}

	// mountains creation
	if (fy <= montainsElevation) {
		if (fy + mapInfo.yFactor * 1 >= montainsElevation)
			return TextureManager::blocksNames["grass"];
		else if (fy + mapInfo.yFactor * 2 >= montainsElevation)
			return TextureManager::blocksNames["dirt"];
		else
			return TextureManager::blocksNames["stone"];
	}

	// sky
	return 0;
}

uint8_t		getBlockNormal(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz) {
	float	x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
	float	y = (chunkPos.y + iy) * mapInfo.yFactor;
	float	z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

	// create normal montains
	float	montainElevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
		+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
		+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
	montainElevation = std::pow(montainElevation, MAP_HEIGHT_EXP);
	montainElevation = montainElevation / MAP_HEIGHT_DIV + MAP_START_HEIGHT;

	// create cavern
	float	cavern;
	cavern = 1 * PERLIN(MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1)
	+  0.5 * PERLIN(MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2)
	+ 0.25 * PERLIN(MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4);
	cavern = std::pow(cavern, MAP_HEIGHT_EXP);
	float cavernY1 = -1;
	float cavernY2 = -1;
	if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
		// there is cavern
		cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.5 + MAP_CAVERN_BASE_Y;
		cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
	}

	// always a block at the first layer
	float	bedrockElevation = PERLIN(10 * x, 10 * z);
	bedrockElevation = std::pow(bedrockElevation, 0.4);
	bedrockElevation *= MAP_MAX_BEDROCK_HEIGHT;
	return _getBlockElevation(chunkPos.y + iy, y, montainElevation, cavernY1, cavernY2, bedrockElevation);
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
	else if (s.m.generationType == GENERATION_FLAT_MAP)
		return getBlockFlatMap(chunkPos, ix, iy, iz);
	else
		return getBlockNormal(chunkPos, ix, iy, iz);
}

void		getChunkVoid(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	(void)chunkPos;
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
			for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
				data[ix][iy][iz] = 0;
			}
		}
	}
}

void		getChunkFlatMap(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
		uint32_t	y = chunkPos.y + iy;
		uint8_t		type = 0;
		for (auto it = s.m.flatMap.begin(); it != s.m.flatMap.end(); it++) {
			if (it->fromY <= y && y <= it->toY) {
				type = TextureManager::blocksNames[it->blockName];
				break;
			}
		}
		for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
			for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
				data[ix][iy][iz] = type;
			}
		}
	}
}

void		getChunkNormal(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	float	x;
	float	y;
	float	z;
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
		for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
			z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

			// create normal montains
			float	montainElevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
				+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
				+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
			montainElevation = std::pow(montainElevation, MAP_HEIGHT_EXP);
			montainElevation = montainElevation / MAP_HEIGHT_DIV + MAP_START_HEIGHT;

			// create cavern 1/2 (the second part is relative to y coordinate)
			float	cavern;
			cavern = 1 * PERLIN(MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1)
			+  0.5 * PERLIN(MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2)
			+ 0.25 * PERLIN(MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4);
			cavern = std::pow(cavern, MAP_HEIGHT_EXP);
			float cavernY1 = -1;
			float cavernY2 = -1;

			// always a block at the first layer
			float	bedrockElevation = PERLIN(10 * x, 10 * z);
			bedrockElevation = std::pow(bedrockElevation, 0.4);
			bedrockElevation *= MAP_MAX_BEDROCK_HEIGHT;

			for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
				y = (chunkPos.y + iy) * mapInfo.yFactor;

				// create cavern 2/2
				if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
					// there is cavern
					cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.5 + MAP_CAVERN_BASE_Y;
					cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
				}
				data[ix][iy][iz] = _getBlockElevation(chunkPos.y + iy, y, montainElevation, cavernY1, cavernY2,
					bedrockElevation);
			}
		}
	}
}

void		getChunk(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	if (s.m.generationType == GENERATION_VOID)
		getChunkVoid(chunkPos, data);
	else if (s.m.generationType == GENERATION_FLAT_MAP)
		getChunkFlatMap(chunkPos, data);
	else
		getChunkNormal(chunkPos, data);
}
