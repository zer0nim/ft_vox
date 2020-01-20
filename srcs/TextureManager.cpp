#include "TextureManager.hpp"

#include <fstream>

#include "utils/Texture.hpp"
#include "Logging.hpp"
#include "ft_vox.hpp"

std::map<std::string, uint8_t>	TextureManager::blocksNames = {
	{"dirt",				1},
	{"grass",				2},
	{"dirt-podzol",			3},
	{"snow-dirt",			4},
	{"snow",				5},
	{"snow-stone",			6},
	{"stone",				7},
	{"cobblestone",			8},
	{"stonebrick",			9},
	{"stonebrick-cracked",	10},
	{"stonebrick-mossy",	11},
	{"gravel",				12},
	{"andesite",			13},
	{"diorite",				14},
	{"granite",				15},
	{"diamond-ore",			16},
	{"coal-ore",			17},
	{"iron-ore",			18},
	{"redstone-ore",		19},
	{"gold-ore",			20},
	{"lapis-lazuli",		21},
	{"emerald-ore",			22},
	{"bedrock",				23},
	{"brick",				24},
	{"glass",				25},
	{"sandstone-smooth-1",	26},
	{"sandstone-smooth-2",	27},
	{"sandstone-carved",	28},
	{"sandstone",			29},
	{"sand",				30},
	{"cactus",				31},
	{"oak-leaves",			32},
	{"oak-wood",			33},
	{"oak-planks",			34},
	{"spruce-wood",			35},
	{"spruce-planks",		36},
	{"spruce-door-upper",	37},
	{"spruce-door-lower",	38},
	{"melon",				39},
	{"pumpkin",				40},
	{"mushroom-body",		41},
	{"mushroom-top",		42},
};

TextureManager::TextureManager(std::string const &texturesSettings) {
	_blocks.fill(nullptr);
	try {
		std::ifstream fileStream(texturesSettings, std::ifstream::in);

		if (fileStream.is_open()) {
			nlohmann::json	data;
			data << fileStream;
			loadBlocksTextures(data);
		}
		else {
			logErr("failed to load texture json");
			throw TextureManager::FailedToOpenException(std::string(texturesSettings));
		}
	}
	catch (const nlohmann::json::parse_error& e) {
		logErr("failed to load texture json" << e.what());
		throw TextureManager::JsonParseException();
	}
}

TextureManager::TextureManager(TextureManager const &src) {
	*this = src;
}

TextureManager::~TextureManager() {
	// destroy _blocks
	for (BlockTexture *block : _blocks) {
		if (block != nullptr) {
			delete block;
		}
	}

	// destroy _texturesLoaded
	if (_textureAtlas != nullptr) {
		delete _textureAtlas;
	}
}

TextureManager &TextureManager::operator=(TextureManager const &rhs) {
	if (this != &rhs) {
		// clone _blocks array
		std::array<TextureManager::BlockTexture *, NB_TYPE_BLOCKS> const &rhsBlocks = rhs.getBlocks();
		for (size_t i = 0; i < rhsBlocks.size(); ++i) {
			_blocks[i] = nullptr;
			if (rhsBlocks[i] != nullptr) {
				_blocks[i] = new TextureManager::BlockTexture();
				_blocks[i]->side = rhsBlocks[i]->side;
				_blocks[i]->top = rhsBlocks[i]->top;
				_blocks[i]->bottom = rhsBlocks[i]->bottom;
			}
		}

		// clone _texturesLoaded
		TextureManager::Texture const *	oldOne = rhs.getTextureAtlas();
		_textureAtlas = new Texture(oldOne->id, oldOne->path);
	}
	return *this;
}

void	TextureManager::loadBlocksTextures(nlohmann::json const &data) {
	// load texture atlas
	_textureAtlas = new TextureManager::Texture();
	std::string path = s.g.files.textures;
	bool inSpaceSRGB = true;
	try {
		_textureAtlas->id = textureAtlasFromFile(path, inSpaceSRGB, 32, 256);
	}
	catch(TextureFailToLoad const & e) {
		delete _textureAtlas;
		throw TextureManager::failed2LoadTextureException();
	}

	// read json data to fill blocks textures infos
	if (data.find("blocks") != data.end()) {
		for (auto &block : data["blocks"].items()) {
			// valid material name
			auto it = blocksNames.find(block.key());
			if (it != blocksNames.end()) {
				uint8_t id = blocksNames[block.key()] - 1;

				// texture has not already been defined
				if (_blocks[id] == nullptr) {
					BlockTexture	*blockTexture = new BlockTexture();

					// fill blockTexture infos
					for (auto &blockText : block.value().items()) {
						std::string	key = blockText.key();
						if (key == "default") {
							blockTexture->side = blockText.value();
						}
						else if (key == "top") {
							blockTexture->top = blockText.value();
						}
						else if (key == "bottom") {
							blockTexture->bottom = blockText.value();
						}
						else if (key == "isTransparent") {  // load transparency of the object
							try {
								blockTexture->isTransparent = blockText.value();
							}
							catch (nlohmann::detail::type_error &e) {
								logErr("unable to load transparency of " << block.key() << " " << blockText.value()
								<< " should be a boolean");
							}
						}
						else {
							logWarn("block \"" << block.key() << "\", unrecognized texture key \"" <<  key  << '"');
						}
					}

					if (blockTexture->side == -1) {
						// free memory
						for (BlockTexture *block : _blocks) {
							if (block != nullptr) {
								delete block;
							}
						}
						delete _textureAtlas;

						logErr("missing default texture for block \"" << block.key() << '"');
						throw TextureManager::missingBlockException();
					}

					_blocks[id] = blockTexture;
				}
			}
			else {
				// free memory
				for (BlockTexture *block : _blocks) {
					if (block != nullptr) {
						delete block;
					}
				}
				delete _textureAtlas;

				logErr("invalid block name in textures.json: " << block.key());
				throw TextureManager::missingBlockException();
			}
		}
	}

	// check for missing block textures info
	bool missingBlock = false;
	for (size_t i = 0; i < _blocks.size(); ++i) {
		if (_blocks[i] == nullptr) {
			missingBlock = true;
			logErr(std::string("missing block: ") << i << " in textures.json");
		}
	}
	if (missingBlock) {
		// free memory
		for (BlockTexture *block : _blocks) {
			if (block != nullptr) {
				delete block;
			}
		}
		delete _textureAtlas;

		throw TextureManager::missingBlockException();
	}
}

void	TextureManager::setUniform(Shader &sh) const {
	// activate textures
	glActiveTexture(GL_TEXTURE0);
	sh.setInt("textureAtlas", 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas->id);

	// set uniforms textures
	for (size_t i = 0; i < _blocks.size(); ++i) {
		sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureSide", _blocks[i]->side);

		// top texture
		if (_blocks[i]->top != -1) {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureTop", _blocks[i]->top);
		}
		else {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureTop", _blocks[i]->side);
		}

		// bottom texture
		if (_blocks[i]->bottom != -1) {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureBottom", _blocks[i]->bottom);
		}
		else {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureBottom", _blocks[i]->side);
		}
	}
}

void	TextureManager::activateTextures() const {
	// activate textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas->id);
}

TextureManager::Texture const *	TextureManager::getTextureAtlas() const {
	return _textureAtlas;
}

std::array<TextureManager::BlockTexture *, NB_TYPE_BLOCKS> const &	TextureManager::getBlocks() const {
	return _blocks;
}

std::ostream & operator << (std::ostream &out, const TextureManager::Texture &m) {
	out << "{ id: " << m.id << ", path: " << m.path << " }";
	return out;
}

std::ostream & operator << (std::ostream &out, const TextureManager &tm) {
	std::array<TextureManager::BlockTexture *, NB_TYPE_BLOCKS> const &blocks = tm.getBlocks();

	for (TextureManager::BlockTexture *b : blocks) {
		if (b != nullptr) {
			out << "{ " << std::endl;
			if (b->side != -1) {
				out << " side: " << b->side << std::endl;
			}
			if (b->top != -1) {
				out << " top: " << b->top << std::endl;
			}
			if (b->bottom != -1) {
				out << " bottom: " << b->bottom << std::endl;
			}
			out << "}" << std::endl;
		}
	}

	return out;
}
