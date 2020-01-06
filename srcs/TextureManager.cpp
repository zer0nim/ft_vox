#include "TextureManager.hpp"

#include <fstream>

#include "utils/Texture.hpp"
#include "Logging.hpp"


std::map<std::string, uint8_t>	TextureManager::blocksNames = {
	{"dirt",		1},
	{"stone",		2},
	{"grass",		3},
	{"sand",		4},
	{"bedrock",		5},
	{"snow",		6},
	{"snow-stone",	7},
	{"snow-dirt",	8},
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
	std::string path = "./assets/textures/blocs.png";
	bool inSpaceSRGB = true;
	try {
		_textureAtlas->id = textureFromFile(path, inSpaceSRGB);
	}
	catch(TextureFailToLoad const & e) {
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
						else {
							logWarn("block \"" << block.key() << "\", unrecognized texture key \"" <<  key  << '"');
						}
					}

					if (blockTexture->side == -1) {
						logErr("missing default texture for block \"" << block.key() << '"');
						throw TextureManager::missingBlockException();
					}

					_blocks[id] = blockTexture;
				}
			}
			else {
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
		throw TextureManager::missingBlockException();
	}
}

void	TextureManager::setUniform(Shader &sh) const {
	// activate textures
	glActiveTexture(GL_TEXTURE0 + 0);
	sh.setInt("textureAtlas", 0);
	glBindTexture(GL_TEXTURE_2D, _textureAtlas->id);

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
	glBindTexture(GL_TEXTURE_2D, _textureAtlas->id);
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
