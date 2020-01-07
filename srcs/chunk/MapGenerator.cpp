#include <limits>
#include "MapGenerator.hpp"
#include "TextureManager.hpp"

static MapInfo mapInfo = MapInfo();

float			fseed;


// tree description
#define NB_TREE_MODELS	6
#define TREE_CACTUS		0
#define TREE_BASIC_1	1
#define TREE_BASIC_2	2
#define TREE_BASIC_3	3
#define TREE_ROC		4
#define TREE_PYRAMID	5
uint8_t	ca = TextureManager::blocksNames["cactus"];  // cactus
uint8_t	ow = TextureManager::blocksNames["oak-wood"];  // oak wood
uint8_t	ol = TextureManager::blocksNames["oak-leaves"];  // oak leaves
uint8_t	cb = TextureManager::blocksNames["cobblestone"];  // cobblestone
uint8_t	ss = TextureManager::blocksNames["sandstone"];  // sand stone
uint8_t	TREE_MODELS[NB_TREE_MODELS][TREE_HEIGHT][TREE_RADIUS * 2 + 1][TREE_RADIUS * 2 + 1] = {
	{  // [0] cactus
		{  // height 0
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 1
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 2
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 3
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 4
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 5
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 6
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 7
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [1] basic tree 1
		{  // height 0
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 1
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 2
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{  // height 3
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{  // height 4
			{ 0,  0,  0,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0, ol, ow, ol,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 5
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 6
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 7
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [2] basic tree 2
		{  // height 0
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 1
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 2
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 3
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ow, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 4
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{  // height 5
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{  // height 6
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{  // height 7
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [3] basic tree 3
		{  // height 0
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 1
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{  // height 2
			{ 0, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{  // height 3
			{ 0,  0,  0,  0,  0},
			{ 0, ol, ol,  0,  0},
			{ 0, ol, ow, ol,  0},
			{ 0, ol, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 4
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 5
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 6
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 7
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [5] roc for mountains (print at -7 in height)
		{  // height 0
			{cb, cb, cb, cb,  0},
			{cb, cb, cb, cb, cb},
			{cb, cb, cb, cb, cb},
			{cb, cb, cb, cb, cb},
			{ 0, cb, cb, cb,  0},
		},
		{  // height 1
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb, cb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 2
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 3
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 4
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 5
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 6
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 7
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [6] pyramid for desert (print at -1 in height)
		{  // height 3
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{  // height 3
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{  // height 0
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{  // height 1
			{ 0,  0,  0,  0,  0},
			{ 0, ss, ss, ss,  0},
			{ 0, ss, ss, ss,  0},
			{ 0, ss, ss, ss,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 2
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ss,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 3
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 4
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{  // height 5
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
};

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
float cavernY1, float cavernY2, float bedrockElevation,
TreeInfo treeMap[CHUNK_SZ_X + TREE_RADIUS * 2][CHUNK_SZ_Z + TREE_RADIUS * 2], uint8_t treeX, uint8_t treeZ,
bool &isTreeXZ) {
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

	// tree generation
	if (isTreeXZ && s.m.generateTree && fy > montainsElevation) {
		bool tmpIsTree = false;
		// check for tree
		for (uint8_t i = treeX - TREE_RADIUS; i <= treeX + TREE_RADIUS; i++) {
			for (uint8_t j = treeZ - TREE_RADIUS; j <= treeZ + TREE_RADIUS; j++) {
				if (treeMap[i][j].isTree && fy < treeMap[i][j].elevation + mapInfo.yFactor * TREE_HEIGHT)
					tmpIsTree = true;
				if (treeMap[i][j].isTree
				&& fy >= treeMap[i][j].elevation && fy < treeMap[i][j].elevation + mapInfo.yFactor * TREE_HEIGHT) {
					// [id][y][x][z]
					uint8_t tree = TREE_MODELS[treeMap[i][j].type]
						[static_cast<uint8_t>((fy - treeMap[i][j].elevation) / mapInfo.yFactor)]  /* y */
						[treeX - i + TREE_RADIUS]  /* x */
						[treeZ - j + TREE_RADIUS]  /* z */;
					if (tree > 0)
						return tree;
				}
			}
		}
		isTreeXZ = tmpIsTree;
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

static uint8_t	getBiome(float x, float z, float &heightDiv) {
	uint8_t	biome;

	// check the biome type
	float	biomef1 = 1 * PERLIN(MAP_BIOME_SIZE * x * 1, MAP_BIOME_SIZE * z * 1)
		+  0.5 * PERLIN(MAP_BIOME_SIZE * x * 2, MAP_BIOME_SIZE * z * 2)
		+ 0.25 * PERLIN(MAP_BIOME_SIZE * x * 4, MAP_BIOME_SIZE * z * 4);
	float	biomef2 = 1 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 1, MAP_BIOME_SIZE * (z + 0.1) * 1)
		+  0.5 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 2, MAP_BIOME_SIZE * (z + 0.1) * 2)
		+ 0.25 * PERLIN(MAP_BIOME_SIZE * (x + 0.1) * 4, MAP_BIOME_SIZE * (z + 0.1) * 4);
	float subf1f2 = biomef1 - biomef2 - MOUNTAIN_OFFSET;  // substraction of f1 & f2

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

	return biome;
}

static float	getMountainsElevation(float x, float z, float heightDiv) {
	float	montainElevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
		+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
		+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
	montainElevation = std::pow(montainElevation, MAP_HEIGHT_EXP);
	montainElevation = montainElevation / heightDiv + MAP_START_HEIGHT;

	return montainElevation;
}

static bool		isTree(int32_t ix, int32_t iz, float perlinX, float perlinZ, float startVal, float density) {
	float tree = PERLIN(perlinX, perlinZ);
	if ((tree > startVal && tree <= startVal + density)) {
		float tree2 = PERLIN(((ix + 1) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5) * 2, perlinZ);
		float tree3 = PERLIN(perlinX, ((iz + 1) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5) * 2);
		if (!((tree2 > startVal && tree2 <= startVal + density) || (tree3 > startVal && tree3 <= startVal + density))) {
			return true;
		}
	}
	return false;
}

void		getChunkNormalPerlin(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	float	x;
	float	y;
	float	z;
	uint8_t	biome;

	TreeInfo	treeMap[CHUNK_SZ_X + TREE_RADIUS * 2][CHUNK_SZ_Z + TREE_RADIUS * 2];

	if (s.m.generateTree) {
		int i = 0;
		for (int8_t ix = -TREE_RADIUS; ix < CHUNK_SZ_X + TREE_RADIUS; ix++) {
			x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
			int j = 0;
			for (int8_t iz = -TREE_RADIUS; iz < CHUNK_SZ_Z + TREE_RADIUS; iz++) {
				z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

				// tree 1
				if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 2, z * 2, 0.1, 0.0005)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC;  // roc
								treeMap[i][j].elevation -= mapInfo.yFactor * 7;
							}
							else {
								treeMap[i][j].isTree = false;  // no roc in snow
							}
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_BASIC_1;  // basic tree
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].type = TREE_CACTUS;  // cactus
						}
				}
				// tree 2
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 3, z * 3, 0.1, 0.0005)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC;  // roc
								treeMap[i][j].elevation -= mapInfo.yFactor * 7;
							}
							else {
								treeMap[i][j].isTree = false;  // no roc in snow
							}
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_BASIC_2;  // basic tree
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].isTree = false;  // no tree in mountains
						}
				}
				// tree 3
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 4, z * 4, 0.1, 0.0005)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC;  // roc
								treeMap[i][j].elevation -= mapInfo.yFactor * 7;
							}
							else {
								treeMap[i][j].isTree = false;  // no roc in snow
							}
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_BASIC_3;  // basic tree
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].isTree = false;  // no tree in mountains
						}
				}
				// structure (pyramid)
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 5, z * 5, 0.6, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							treeMap[i][j].isTree = false;  // no structure in mountains
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].isTree = false;  // no structure in plain
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].type = TREE_PYRAMID;  // pyramid
							treeMap[i][j].elevation -= mapInfo.yFactor * 1;
						}
				}
				j++;
			}
			i++;
		}
	}

	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		x = (chunkPos.x + ix) * mapInfo.xFactor * NORMALIZE_MULTIPLIER + 0.5;
		for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
			z = (chunkPos.z + iz) * mapInfo.zFactor * NORMALIZE_MULTIPLIER + 0.5;

			// check the biome
			float heightDiv;
			biome = getBiome(x, z, heightDiv);

			// create normal montains
			float	montainElevation = getMountainsElevation(x, z, heightDiv);

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

			// used to reduce calculation in _getBlockElevation
			bool isTreeXZ = true;  // true if we need to check if there is a tree in this position

			for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
				y = (chunkPos.y + iy) * mapInfo.yFactor;

				// create cavern 2/2
				if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
					// there is cavern
					cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.2 + MAP_CAVERN_BASE_Y;
					cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
				}
				data[ix][iy][iz] = _getBlockElevation(chunkPos.y + iy, y, biome, montainElevation, cavernY1, cavernY2,
					bedrockElevation, treeMap, ix + TREE_RADIUS, iz + TREE_RADIUS, isTreeXZ);
			}
		}
	}
}

void		getChunkNormal(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	getChunkNormalPerlin(chunkPos, data);
}

void		getChunk(wordIVec3 &chunkPos, uint8_t data[CHUNK_SZ_X][CHUNK_SZ_Y][CHUNK_SZ_Z]) {
	if (s.m.generationType == GENERATION_VOID)
		getChunkVoid(chunkPos, data);
	else if (s.m.generationType == GENERATION_FLAT_MAP)
		getChunkFlatMap(chunkPos, data);
	else
		getChunkNormal(chunkPos, data);
}
