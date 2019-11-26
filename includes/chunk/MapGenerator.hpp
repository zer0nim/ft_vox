#pragma once

#include "ft_vox.hpp"
#include "lib/PerlinNoise.hpp"

#define NORMALIZE_MULTIPLIER	910
/* montains generation */
#define MAP_FREQ_MONTAIN		0.5		// frequency -> nb of mountain
#define MAP_HEIGHT_EXP			1		// bigger nb == small mountains
#define MAP_START_HEIGHT		0.2		// [0..1] start at 0.5 (half of the height)
/* cavern generation */
#define MAP_CAVERN_Y_MULTIPLIER	3		// multiply Y by ... in cavern
#define MAP_CAVERN_FREQ			0.3		// frequency of caverns
#define MAP_CAVERN_BASE_Y		0.1		// [0..1] base y for cavern height
#define MAP_CAVERN_HEIGHT		0.15    // [0..1] height of a cavern
#define MAP_CAVERN_HEIGHT_EXP	1		// bigger nb == smaller cavern
#define MAP_CAVERN_START		0.15    // [0..1] start cavern at this perlin value
#define MAP_CAVERN_END			0.4		// [0..1] end cavern at this perlin value
/* bedrock */
#define MAP_MAX_BEDROCK_HEIGHT	3		// [block] max bedrock height

/* biomes */
// biome size and offsets
#define MAP_BIOME_SIZE				0.04 	// reverse of the size
#define MOUNTAIN_OFFSET				0.003 	// less number == larger mountains biomes
#define PLAIN_OFFSET				0.005 	// less numer == larger plain biome (transition biome)

// biomes mountains height (bigger nb == smaller mountains)
#define MAP_HEIGHT_DIV_PLAIN		20		// height divisor (to avoid big montains)
#define MAP_HEIGHT_DIV_DESERT		40		// height divisor (to avoid big montains)
#define MAP_HEIGHT_DIV_MOUNTAINS	10		// height divisor (to avoid big montains)


// biomes ID
#define MAP_BIOME_PLAIN		0
#define MAP_BIOME_DESERT	1
#define MAP_BIOME_MOUNTAINS	2

#define PERLIN mapInfo.perlin.noise     // shortcut to perlin call

struct MapInfo {
	float				xFactor;
	float				yFactor;
	float				zFactor;
	siv::PerlinNoise	perlin;

	MapInfo() :
		xFactor(1.0 / (MAX_MAP_SIZE_X - 1)),
		yFactor(1.0 / (MAX_MAP_SIZE_Y - 1)),
		zFactor(1.0 / (MAX_MAP_SIZE_Z - 1)),
		perlin(1) {}
};

void		setSeed(uint32_t seed);

void		getChunkVoid(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]);
void		getChunkFlatMap(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]);
void		getChunkNormal(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]);
void		getChunk(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]);
