#include <limits>
#include "MapGenerator.hpp"
#include "TextureManager.hpp"

static MapInfo mapInfo = MapInfo();

float			fseed;


// tree description
enum TREE_TYPE {
	TREE_CACTUS,
	TREE_BASIC_1,
	TREE_BASIC_2,
	TREE_BASIC_3,
	TREE_ROC_1,
	TREE_DESERT_PYRAMID,
	TREE_ROC_2,
	TREE_MUSHROOM,
	TREE_MELON,
	TREE_PUMPKIN,
	TREE_PLAIN_HOUSE,
	TREE_PLAIN_TOWER,
	TREE_DESERT_TOWER,
	TREE_DESERT_TEMPLE,
	TREE_MOUNTAIN_TOWER,
	NB_TREE_MODELS,
};
uint8_t	ca = TextureManager::blocksNames["cactus"];
uint8_t	ow = TextureManager::blocksNames["oak-wood"];
uint8_t	sw = TextureManager::blocksNames["spruce-wood"];
uint8_t	ol = TextureManager::blocksNames["oak-leaves"];
uint8_t	cb = TextureManager::blocksNames["cobblestone"];
uint8_t	gr = TextureManager::blocksNames["gravel"];
uint8_t	sd = TextureManager::blocksNames["sand"];
uint8_t	ss = TextureManager::blocksNames["sandstone"];
uint8_t	s1 = TextureManager::blocksNames["sandstone-smooth-1"];
uint8_t	s2 = TextureManager::blocksNames["sandstone-smooth-2"];
uint8_t	s3 = TextureManager::blocksNames["sandstone-carved"];
uint8_t	gl = TextureManager::blocksNames["glass"];
uint8_t	mb = TextureManager::blocksNames["mushroom-body"];
uint8_t	mt = TextureManager::blocksNames["mushroom-top"];
uint8_t	di = TextureManager::blocksNames["dirt"];
uint8_t	dp = TextureManager::blocksNames["dirt-podzol"];
uint8_t	me = TextureManager::blocksNames["melon"];
uint8_t	pk = TextureManager::blocksNames["pumpkin"];
uint8_t	sb = TextureManager::blocksNames["stonebrick"];
uint8_t	br = TextureManager::blocksNames["brick"];
uint8_t	op = TextureManager::blocksNames["oak-planks"];
uint8_t	sc = TextureManager::blocksNames["stonebrick-cracked"];
uint8_t	sm = TextureManager::blocksNames["stonebrick-mossy"];
uint8_t	dl = TextureManager::blocksNames["spruce-door-lower"];
uint8_t	du = TextureManager::blocksNames["spruce-door-upper"];
uint8_t	TREE_MODELS[NB_TREE_MODELS][TREE_HEIGHT][TREE_RADIUS * 2 + 1][TREE_RADIUS * 2 + 1] = {
	{  // [0] cactus
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ca,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [1] basic tree 1
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, sw,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, sw,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, sw, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, sw, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0, ol, sw, ol,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [2] basic tree 2
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ow, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ 0, ol, ol, ol,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [3] basic tree 3
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ow,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0, ol, ol, ol,  0},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{
			{ 0, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ow, ol, ol},
			{ol, ol, ol, ol, ol},
			{ol, ol, ol, ol, ol},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, ol, ol,  0,  0},
			{ 0, ol, ow, ol,  0},
			{ 0, ol, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ol,  0,  0},
			{ 0, ol, ol, ol,  0},
			{ 0,  0, ol,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [4] roc 1 for mountains (print at -7 in height)
		{
			{cb, cb, cb, cb,  0},
			{cb, cb, cb, cb, cb},
			{cb, cb, cb, cb, cb},
			{cb, cb, cb, cb, cb},
			{ 0, cb, cb, cb,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb, cb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0, cb, cb, cb,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, cb,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [5] pyramid for desert (print at -2 in height)
		{
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{ss, ss, ss, ss, ss},
			{ss,  0,  0,  0, ss},
			{ss,  0,  0,  0, ss},
			{ss,  0,  0,  0, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{ss, ss, ss, ss, ss},
			{ss,  0,  0,  0, ss},
			{ss,  0,  0,  0, ss},
			{ss,  0,  0,  0, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, ss, ss, ss,  0},
			{ 0, ss,  0, ss,  0},
			{ 0, ss, ss, ss,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, ss,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [6] roc 2 for mountains (print at -7 in height)
		{
			{gr, gr, gr, gr,  0},
			{gr, gr, gr, gr, gr},
			{gr, gr, gr, gr, gr},
			{gr, gr, gr, gr, gr},
			{ 0, gr, gr, gr,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr, gr,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, gr,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, gr,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0, gr,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0, gr, gr, gr,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, gr,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [7] mushroom (print at -1 in height)
		{
			{ 0,  0,  0,  0,  0},
			{ 0, dp, dp, dp,  0},
			{ 0, dp, dp, dp,  0},
			{ 0, dp, dp, dp,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, mb,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, mb,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0, mt, mt, mt,  0},
			{mt,  0,  0,  0, mt},
			{mt,  0, mb,  0, mt},
			{mt,  0,  0,  0, mt},
			{ 0, mt, mt, mt,  0},
		},
		{
			{ 0, mt, mt, mt,  0},
			{mt,  0,  0,  0, mt},
			{mt,  0, mb,  0, mt},
			{mt,  0,  0,  0, mt},
			{ 0, mt, mt, mt,  0},
		},
		{
			{ 0, mt, mt, mt,  0},
			{mt,  0,  0,  0, mt},
			{mt,  0, mb,  0, mt},
			{mt,  0,  0,  0, mt},
			{ 0, mt, mt, mt,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, mt, mt, mt,  0},
			{ 0, mt, mt, mt,  0},
			{ 0, mt, mt, mt,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [8] melon
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, me,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [9] pumpkin
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, pk,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [10] plain-house (print at -2 in height)
		{
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
		},
		{
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
		},
		{
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
			{di, di, di, di, di},
		},
		{
			{br, br, br, br, br},
			{br, op, op, op, br},
			{br, op, op, op, br},
			{br, op, op, op, br},
			{br, br, br, br, br},
		},
		{
			{br, br, gl, br, br},
			{br,  0,  0,  0, br},
			{br,  0,  0,  0, br},
			{br,  0,  0,  0, br},
			{br, br, gl, br, br},
		},
		{
			{op, op, gl, op, op},
			{op,  0,  0,  0, op},
			{op,  0,  0,  0, op},
			{op,  0,  0,  0, op},
			{op, op, gl, op, op},
		},
		{
			{ 0, op, op, op,  0},
			{ 0, op,  0, op,  0},
			{ 0, op,  0, op,  0},
			{ 0, op,  0, op,  0},
			{ 0, op, op, op,  0},
		},
		{
			{ 0,  0, op,  0,  0},
			{ 0,  0, op,  0,  0},
			{ 0,  0, op,  0,  0},
			{ 0,  0, op,  0,  0},
			{ 0,  0, op,  0,  0},
		},
	},
	{  // [11] plain-tower (print at -2 in height)
		{
			{sb, sc, sb, sb, sb},
			{sb, sb, sb, sb, sm},
			{sb, sb, sc, sm, sc},
			{sc, sb, sb, sm, sm},
			{sc, sm, sm, sb, sm},
		},
		{
			{sb, sc, sc, sm, sm},
			{sm, sb, sc, sb, sm},
			{sm, sm, sc, sb, sc},
			{sb, sm, sm, sb, sb},
			{sm, sb, sb, sm, sb},
		},
		{
			{sm, sb, sm, sb, sb},
			{sm, br, br, br, sb},
			{sb, br, br, br, sb},
			{sb, br, br, br, sb},
			{sb, sc, sm, sm, sm},
		},
		{
			{sm, sc, sc, sb, sb},
			{sm, op, op, op, sb},
			{sb, op, op, op, sm},
			{sc, op, op, op, sc},
			{sb, sm, sb, sc, sc},
		},
		{
			{sb, sc, sb, sb, sb},
			{sb,  0,  0,  0, sc},
			{sc,  0,  0,  0, sb},
			{sc,  0,  0,  0, sb},
			{sb, sb, sc, sb, sb},
		},
		{
			{sc, gl, sb, gl, sc},
			{gl,  0,  0,  0, gl},
			{sc,  0,  0,  0, sb},
			{gl,  0,  0,  0, gl},
			{sb, gl, sc, gl, sb},
		},
		{
			{sb, sb, sb, sb, sc},
			{sc, sc, sb, sb, sb},
			{sc, sb, sc, sc, sb},
			{sb, sb, sc, sb, sc},
			{sb, sb, sc, sb, sc},
		},
		{
			{sc,  0, sc,  0, sb},
			{ 0,  0,  0,  0,  0},
			{sb,  0,  0,  0, sc},
			{ 0,  0,  0,  0,  0},
			{sb,  0, sc,  0, sb},
		},
	},
	{  // [12] desert-tower (print at -3 in height)
		{
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
			{ss, ss, ss, ss, ss},
		},
		{
			{s2, s1, s2, s1, s2},
			{s1,  0,  0,  0, s1},
			{s2,  0,  0,  0, s2},
			{s1,  0,  0,  0, s1},
			{s2, s1, dl, s1, s2},
		},
		{
			{s2, s1, s2, s1, s2},
			{s1,  0,  0,  0, s1},
			{s2,  0,  0,  0, s2},
			{s1,  0,  0,  0, s1},
			{s2, s1, du, s1, s2},
		},
		{
			{s2, s3, s2, s3, s2},
			{s3,  0,  0,  0, s3},
			{s2,  0,  0,  0, s2},
			{s3,  0,  0,  0, s3},
			{s2, s3, s2, s3, s2},
		},
		{
			{s1, s1, s1, s1, s1},
			{s1, s1, s1, s1, s1},
			{s1, s1, s1, s1, s1},
			{s1, s1, s1, s1, s1},
			{s1, s1, s1, s1, s1},
		},
		{
			{s1,  0, s1,  0, s1},
			{ 0,  0,  0,  0,  0},
			{s1,  0,  0,  0, s1},
			{ 0,  0,  0,  0,  0},
			{s1,  0, s1,  0, s1},
		},
	},
	{  // [13] desert-temple (print at -2 in height)
		{
			{sd, sd, sd, sd, sd},
			{sd, sd, sd, sd, sd},
			{sd, sd, sd, sd, sd},
			{sd, sd, sd, sd, sd},
			{sd, sd, sd, sd, sd},
		},
		{
			{sd, sd, ss, sd, sd},
			{sd, sd, ss, sd, sd},
			{ss, ss, ss, ss, ss},
			{sd, sd, ss, sd, sd},
			{sd, sd, ss, sd, sd},
		},
		{
			{ss, ss,  0, ss, ss},
			{ss,  0,  0,  0, ss},
			{ 0,  0,  0,  0,  0},
			{ss,  0,  0,  0, ss},
			{ss, ss,  0, ss, ss},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, s1,  0, s1,  0},
			{ 0,  0,  0,  0,  0},
			{ 0, s1,  0, s1,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, s1, s3, s1,  0},
			{ 0, s3,  0, s3,  0},
			{ 0, s1, s3, s1,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0, s1,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
			{ 0,  0,  0,  0,  0},
		},
	},
	{  // [14] mountain-tower (print at -2 in height)
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sc, sc, sb,  0},
			{ 0, sb, sc, sb,  0},
			{ 0, sb, sb, sb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sb, sc, sb,  0},
			{ 0, sc, sb, sc,  0},
			{ 0, sb, sb, sc,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sb, sc, sb,  0},
			{ 0, sb, sc, sb,  0},
			{ 0, sb, sc, sb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sc, sc, sb,  0},
			{ 0, sc, sb, sb,  0},
			{ 0, sb, sb, sc,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sb, sb, sc,  0},
			{ 0, sb,  0, sc,  0},
			{ 0, sb, sc, sb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sb, gl, sb,  0},
			{ 0, gl,  0, gl,  0},
			{ 0, sc, gl, sc,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sc, sb, sb,  0},
			{ 0, sb, sc, sc,  0},
			{ 0, sb, sc, sb,  0},
			{ 0,  0,  0,  0,  0},
		},
		{
			{ 0,  0,  0,  0,  0},
			{ 0, sb,  0, sb,  0},
			{ 0,  0,  0,  0,  0},
			{ 0, sb,  0, sc,  0},
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
float cavernY1, float cavernY2, float bedrockElevation, uint8_t stoneOrOre,
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
			else if (fy + mapInfo.yFactor * 3 >= montainsElevation)
				return TextureManager::blocksNames["stone"];
			return stoneOrOre;
		}
		else if (biome == MAP_BIOME_DESERT) {
			if (fy + mapInfo.yFactor * 2 >= montainsElevation)
				return TextureManager::blocksNames["sand"];
			else if (fy + mapInfo.yFactor * 3 >= montainsElevation)
				return TextureManager::blocksNames["sandstone"];
			return stoneOrOre;
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
			return stoneOrOre;
		}
	}

	// tree generation
	if (isTreeXZ && s.m.generateTree && fy > montainsElevation) {
		bool tmpIsTree = false;
		// check for tree
		uint8_t	returnValue = 0;
		int8_t	returnPriority = -1;
		for (uint8_t i = treeX - TREE_RADIUS; i <= treeX + TREE_RADIUS; i++) {
			for (uint8_t j = treeZ - TREE_RADIUS; j <= treeZ + TREE_RADIUS; j++) {
				if (treeMap[i][j].isTree && fy < treeMap[i][j].elevation + mapInfo.yFactor * TREE_HEIGHT)
					tmpIsTree = true;
				if (treeMap[i][j].isTree && treeMap[i][j].priorityLvl > returnPriority
				&& fy >= treeMap[i][j].elevation && fy < treeMap[i][j].elevation + mapInfo.yFactor * TREE_HEIGHT) {
					// [id][y][x][z]
					uint8_t tree = TREE_MODELS[treeMap[i][j].type]
						[static_cast<uint8_t>((fy - treeMap[i][j].elevation) / mapInfo.yFactor)]  /* y */
						[treeX - i + TREE_RADIUS]  /* x */
						[treeZ - j + TREE_RADIUS]  /* z */;
					if (tree > 0) {
						returnPriority = treeMap[i][j].priorityLvl;
						returnValue = tree;
					}
				}
			}
		}
		if (returnValue > 0)
			return returnValue;
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
	float	montainsElevation = 1 * PERLIN(MAP_FREQ_MONTAIN * x * 1, MAP_FREQ_MONTAIN * z * 1)
		+  0.5 * PERLIN(MAP_FREQ_MONTAIN * x * 2, MAP_FREQ_MONTAIN * z * 2)
		+ 0.25 * PERLIN(MAP_FREQ_MONTAIN * x * 4, MAP_FREQ_MONTAIN * z * 4);
	montainsElevation = std::pow(montainsElevation, MAP_HEIGHT_EXP);
	montainsElevation = montainsElevation / heightDiv + MAP_START_HEIGHT;

	return montainsElevation;
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

static uint8_t	getStoneOrOre(float x, float y, float z, float realY) {
	float tmpX = x * 2;
	float tmpY = y * 20;
	float tmpZ = z * 2;
	float	oreValue = 1 * PERLIN(tmpX * 1, tmpY * 1, tmpZ * 1);

	// diamond
	if (realY < 10 && oreValue > 0.4 && oreValue < 0.41)
		return TextureManager::blocksNames["diamond-ore"];
	// lapis
	if (realY < 25 && oreValue > 0.3 && oreValue < 0.305)
		return TextureManager::blocksNames["lapis-lazuli"];
	// gold
	if (realY < 30 && oreValue > 0.1 && oreValue < 0.102)
		return TextureManager::blocksNames["gold-ore"];
	// coal
	if (oreValue > 0.6 && oreValue < 0.65)
		return TextureManager::blocksNames["coal-ore"];
	// iron
	if (oreValue > 0.41 && oreValue < 0.43)
		return TextureManager::blocksNames["iron-ore"];
	// redstone
	if (oreValue > 0.7 && oreValue < 0.702)
		return TextureManager::blocksNames["redstone-ore"];
	// emerald
	if (oreValue > 0.05 && oreValue < 0.051)
		return TextureManager::blocksNames["emerald-ore"];
	// andesite
	if (oreValue > 0.1 && oreValue < 0.15)
		return TextureManager::blocksNames["andesite"];
	// diorite
	if (oreValue > 0.3 && oreValue < 0.4)
		return TextureManager::blocksNames["diorite"];
	// granite
	if (oreValue > 0.5 && oreValue < 0.99)
		return TextureManager::blocksNames["granite"];
	// gravel
	if (oreValue > 0.01 && oreValue < 0.06)
		return TextureManager::blocksNames["gravel"];
	return TextureManager::blocksNames["stone"];
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
				if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 2, z * 2, 0.1, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC_1;  // roc
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
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 3, z * 3, 0.1, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC_2;  // roc
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
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 4, z * 4, 0.1, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC_2;  // roc
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
				// tree 4
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 5, z * 5, 0.1, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC_1;  // roc
								treeMap[i][j].elevation -= mapInfo.yFactor * 7;
							}
							else {
								treeMap[i][j].isTree = false;  // no roc in snow
							}
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_MELON;  // basic tree
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].isTree = false;  // no tree in mountains
						}
				}
				// tree 5
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 6, z * 6, 0.1, 0.001)) {
						treeMap[i][j].isTree = true;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							if (treeMap[i][j].elevation < MAP_TOP_SNOW_HEIGHT * mapInfo.yFactor) {
								treeMap[i][j].type = TREE_ROC_2;  // roc
								treeMap[i][j].elevation -= mapInfo.yFactor * 7;
							}
							else {
								treeMap[i][j].isTree = false;  // no roc in snow
							}
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_PUMPKIN;  // basic tree
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].isTree = false;  // no tree in mountains
						}
				}
				// structure 1
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 7, z * 7, 0.6, 0.001)) {
						treeMap[i][j].isTree = true;
						treeMap[i][j].priorityLvl = 1;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							treeMap[i][j].type = TREE_MOUNTAIN_TOWER;
							treeMap[i][j].elevation -= mapInfo.yFactor * 2;
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_MUSHROOM;  // mushroom
							treeMap[i][j].elevation -= mapInfo.yFactor * 1;
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].type = TREE_DESERT_PYRAMID;  // pyramid
							treeMap[i][j].elevation -= mapInfo.yFactor * 2;
						}
				}
				// structure 2
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 8, z * 8, 0.6, 0.001)) {
						treeMap[i][j].isTree = true;
						treeMap[i][j].priorityLvl = 1;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							treeMap[i][j].isTree = false;
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].isTree = false;
							// treeMap[i][j].type = TREE_PLAIN_HOUSE;  // plain house
							// treeMap[i][j].elevation -= mapInfo.yFactor * 2;
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].type = TREE_DESERT_TOWER;
							treeMap[i][j].elevation -= mapInfo.yFactor * 3;
						}
				}
				// structure 2
				else if (isTree(chunkPos.x + ix, chunkPos.z + iz, x * 9, z * 9, 0.6, 0.001)) {
						treeMap[i][j].isTree = true;
						treeMap[i][j].priorityLvl = 1;
						float heightDiv;
						float biome = getBiome(x, z, heightDiv);
						treeMap[i][j].elevation = getMountainsElevation(x, z, heightDiv);

						if (biome == MAP_BIOME_MOUNTAINS) {
							treeMap[i][j].isTree = false;  // no structure in mountains
						}
						else if (biome == MAP_BIOME_PLAIN) {
							treeMap[i][j].type = TREE_PLAIN_TOWER;  // plain tower
							treeMap[i][j].elevation -= mapInfo.yFactor * 2;
						}
						else if (biome == MAP_BIOME_DESERT) {
							treeMap[i][j].type = TREE_DESERT_TEMPLE;
							treeMap[i][j].elevation -= mapInfo.yFactor * 2;
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
			float	montainsElevation = getMountainsElevation(x, z, heightDiv);

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

			// when put stone, we can put an ore instead of the stone
			uint8_t	stoneOrOre = TextureManager::blocksNames["stone"];

			for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
				y = (chunkPos.y + iy) * mapInfo.yFactor;

				// create cavern 2/2
				if ((cavern > MAP_CAVERN_START && cavern < MAP_CAVERN_END)) {
					// there is cavern
					cavernY1 = (PERLIN(x, y, z) + MAP_CAVERN_BASE_Y) * 0.2 + MAP_CAVERN_BASE_Y;
					cavernY2 = cavernY1 + (MAP_CAVERN_HEIGHT * 0.5 + (cavern - MAP_CAVERN_START) * 0.5);
				}

				if (chunkPos.y + iy > 2 && y + mapInfo.yFactor * 3 < montainsElevation)
					stoneOrOre = getStoneOrOre(x, y, z, chunkPos.y + iy);

				data[ix][iy][iz] = _getBlockElevation(chunkPos.y + iy, y, biome, montainsElevation, cavernY1, cavernY2,
					bedrockElevation, stoneOrOre, treeMap, ix + TREE_RADIUS, iz + TREE_RADIUS, isTreeXZ);
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
