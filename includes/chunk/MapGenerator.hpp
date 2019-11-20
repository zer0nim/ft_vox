#pragma once

#include "ft_vox.hpp"
#include "lib/PerlinNoise.hpp"

/* montains generation */
#define MAP_FREQ_MONTAIN		0.5		// frequency -> nb of mountain
#define MAP_HEIGHT_EXP			1		// bigger nb == small mountains
#define MAP_START_HEIGHT		0.5		// start at 0.5 (half of the height)
#define MAP_HEIGHT_DIV			3		// height divisor (to avoid big montains)
/* cavern generation */
#define MAP_CAVERN_FREQ			0.3		// frequency of caverns
#define MAP_CAVERN_MAX_HEIGHT	0.4		// max height for cavern

#define PERLIN mapInfo.perlin.noise  // shortcut to perlin call

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
uint8_t		getBlock(wordIVec3 &chunkPos, uint8_t ix, uint8_t iy, uint8_t iz);
