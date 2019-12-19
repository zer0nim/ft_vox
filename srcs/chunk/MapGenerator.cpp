#include <limits>
#include <glm/gtc/noise.hpp>
#include "MapGenerator.hpp"
#include "TextureManager.hpp"

static MapInfo mapInfo = MapInfo();

float			fseed;

void		setSeed(uint32_t seed) {
	#if NOISE_USED == USING_PERLIN
		mapInfo.perlin.reseed(seed);
	#else
		std::mt19937 engine(seed);
		std::uniform_real_distribution<> dist;
		fseed = dist(engine) * 100;
	#endif
}

static uint8_t	_getBlockElevation(uint32_t realY, float fy, uint8_t biome, float montainsElevation,
float cavernY1, float cavernY2, float bedrockElevation) {
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
		if (biome == MAP_BIOME_MOUNTAINS) {
			if (realY >= MAP_FULL_SNOW_HEIGHT && fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["snow"];
			else if (realY >= MAP_FULL_SNOW_HEIGHT - 1 && fy + mapInfo.yFactor * 2 >= montainsElevation)
				return TextureManager::blocksNames["snow-stone"];
			else if (realY >= MAP_TOP_SNOW_HEIGHT && fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["snow-stone"];
			return TextureManager::blocksNames["stone"];
		}
		else if (biome == MAP_BIOME_DESERT) {
			if (fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["sand"];
			return TextureManager::blocksNames["stone"];
		}
		else {  // plain (transition)
			if (realY >= MAP_FULL_SNOW_HEIGHT && fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["snow"];
			else if (realY >= MAP_FULL_SNOW_HEIGHT - 1 && fy + mapInfo.yFactor * 2 >= montainsElevation)
				return TextureManager::blocksNames["snow-dirt"];
			else if (realY >= MAP_TOP_SNOW_HEIGHT && fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["snow-dirt"];
			else if (fy + mapInfo.yFactor * 1 >= montainsElevation)
				return TextureManager::blocksNames["grass"];
			else if (fy + mapInfo.yFactor * 2 >= montainsElevation)
				return TextureManager::blocksNames["dirt"];
			return TextureManager::blocksNames["stone"];
		}
	}

	// sky
	return 0;
}

void		getChunkVoid(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
			for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
				if (chunkPos.x == 0 && chunkPos.y + iy == 0 && chunkPos.z  == 0)
					data[ix][iy][iz] = TextureManager::blocksNames["stone"];
				else
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

void		getChunkNormalPerlin(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	float	x;
	float	y;
	float	z;
	uint8_t	biome;
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
		for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
			z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

			// check the biome type
			float	biomef1 = 1 * PERLIN(MAP_BIOME_SIZE * x * 1, MAP_BIOME_SIZE * z * 1)
				+  0.5 * PERLIN(MAP_BIOME_SIZE * x * 2, MAP_BIOME_SIZE * z * 2)
				+ 0.25 * PERLIN(MAP_BIOME_SIZE * x * 4, MAP_BIOME_SIZE * z * 4);
			float	biomef2 = 1 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 1, MAP_BIOME_SIZE * (z + 0.1) * 1)
				+  0.5 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 2, MAP_BIOME_SIZE * (z + 0.1) * 2)
				+ 0.25 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 4, MAP_BIOME_SIZE * (z + 0.1) * 4);
			float subf1f2 = biomef1 - biomef2 - MOUNTAIN_OFFSET;  // substraction of f1 & f2

			float heightDiv;
			if (subf1f2 > 0) {
				biome = MAP_BIOME_MOUNTAINS;
				heightDiv = MAP_HEIGHT_DIV_MOUNTAINS;
			}
			else if (-PLAIN_OFFSET < subf1f2) {
				float factor = -subf1f2 / PLAIN_OFFSET;  // btw 0 & 1
				biome = MAP_BIOME_PLAIN;
				heightDiv = MAP_HEIGHT_DIV_MOUNTAINS + (MAP_HEIGHT_DIV_DESERT - MAP_HEIGHT_DIV_MOUNTAINS) * factor;
			}
			else {
				biome = MAP_BIOME_DESERT;
				heightDiv = MAP_HEIGHT_DIV_DESERT;
			}

			// create normal montains
			float	montainElevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
				+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
				+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
			montainElevation = std::pow(montainElevation, MAP_HEIGHT_EXP);
			montainElevation = montainElevation / heightDiv + MAP_START_HEIGHT;

			// create cavern 1/2 (the second part is relative to y coordinate)
			float	cavern;
			cavern = 1 * PERLIN(MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1)
			+  0.5 * PERLIN(MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2)
			+ 0.25 * PERLIN(MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4);
			cavern = std::pow(cavern, MAP_CAVERN_HEIGHT_EXP);
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
					cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.2 + MAP_CAVERN_BASE_Y;
					cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
				}
				data[ix][iy][iz] = _getBlockElevation(chunkPos.y + iy, y, biome, montainElevation, cavernY1, cavernY2,
					bedrockElevation);
			}
		}
	}
}

void		getChunkNormalSimplex(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	float	x;
	float	y;
	float	z;
	uint8_t	biome;
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
		for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
			z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

			// check the biome type
			float	biomef1 = 1 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * x * 1, MAP_BIOME_SIZE * z * 1))
				+  0.5 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * x * 2, MAP_BIOME_SIZE * z * 2))
				+ 0.25 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * x * 4, MAP_BIOME_SIZE * z * 4));
			float	biomef2 = 1 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * (x + 0.1) * 1, MAP_BIOME_SIZE * (z + 0.1) * 1))
				+  0.5 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * (x + 0.1) * 2, MAP_BIOME_SIZE * (z + 0.1) * 2))
				+ 0.25 * SIMPLEX(glm::vec3(fseed, MAP_BIOME_SIZE * (x + 0.1) * 4, MAP_BIOME_SIZE * (z + 0.1) * 4));
			float subf1f2 = biomef1 - biomef2 - MOUNTAIN_OFFSET;  // substraction of f1 & f2

			float heightDiv;
			if (subf1f2 > 0) {
				biome = MAP_BIOME_MOUNTAINS;
				heightDiv = MAP_HEIGHT_DIV_MOUNTAINS;
			}
			else if (-PLAIN_OFFSET < subf1f2) {
				float factor = -subf1f2 / PLAIN_OFFSET;  // btw 0 & 1
				biome = MAP_BIOME_PLAIN;
				heightDiv = MAP_HEIGHT_DIV_MOUNTAINS + (MAP_HEIGHT_DIV_DESERT - MAP_HEIGHT_DIV_MOUNTAINS) * factor;
			}
			else {
				biome = MAP_BIOME_DESERT;
				heightDiv = MAP_HEIGHT_DIV_DESERT;
			}

			// create normal montains
			float	montainElevation = 1 * SIMPLEX(glm::vec3(fseed, MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1))
				+  0.5 * SIMPLEX(glm::vec3(fseed, MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2))
				+ 0.25 * SIMPLEX(glm::vec3(fseed, MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4));
			montainElevation = std::pow(montainElevation, MAP_HEIGHT_EXP);
			montainElevation = montainElevation / heightDiv + MAP_START_HEIGHT;

			// create cavern 1/2 (the second part is relative to y coordinate)
			float	cavern;
			cavern = 1 * SIMPLEX(glm::vec3(fseed, MAP_CAVERN_FREQ * x * 1, MAP_CAVERN_FREQ * z * 1))
			+  0.5 * SIMPLEX(glm::vec3(fseed, MAP_CAVERN_FREQ * x * 2, MAP_CAVERN_FREQ * z * 2))
			+ 0.25 * SIMPLEX(glm::vec3(fseed, MAP_CAVERN_FREQ * x * 4, MAP_CAVERN_FREQ * z * 4));
			cavern = std::pow(cavern, MAP_CAVERN_HEIGHT_EXP);
			float cavernY1 = -1;
			float cavernY2 = -1;

			// always a block at the first layer
			float	bedrockElevation = SIMPLEX(glm::vec3(fseed, 10 * x, 10 * z));
			bedrockElevation = std::pow(bedrockElevation, 0.6);
			bedrockElevation *= MAP_MAX_BEDROCK_HEIGHT;

			for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
				y = (chunkPos.y + iy) * mapInfo.yFactor;

				// create cavern 2/2
				if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
					// there is cavern
					cavernY1 = (SIMPLEX(glm::vec4(fseed, x, y, z)) + MAP_CAVERN_BASE_Y) * 0.2 + MAP_CAVERN_BASE_Y;
					cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
				}
				data[ix][iy][iz] = _getBlockElevation(chunkPos.y + iy, y, biome, montainElevation, cavernY1, cavernY2,
					bedrockElevation);
			}
		}
	}
}

void		getChunkNormal(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	#if NOISE_USED == USING_PERLIN
		getChunkNormalPerlin(chunkPos, data);
	#else
		getChunkNormalSimplex(chunkPos, data);
	#endif
}

void		getChunk(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	if (s.m.generationType == GENERATION_VOID)
		getChunkVoid(chunkPos, data);
	else if (s.m.generationType == GENERATION_FLAT_MAP)
		getChunkFlatMap(chunkPos, data);
	else
		getChunkNormal(chunkPos, data);
}
