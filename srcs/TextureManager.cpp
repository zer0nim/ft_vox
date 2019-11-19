#include "TextureManager.hpp"

#include <fstream>

#include "utils/Texture.hpp"

const std::array<std::string, 4>	TextureManager::_blocksNames = {
	"dirt",
	"stone",
	"grass",
	"sand"
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
			std::cout << "throw FailedToOpenException" << std::endl;
			throw TextureManager::FailedToOpenException(std::string(texturesSettings));
		}
	}
	catch (const nlohmann::json::parse_error& e) {
		std::cerr << "message: " << e.what() << std::endl;
		std::cerr << "exception id: " << e.id << std::endl;
		std::cerr << "byte position of error: " << e.byte << std::endl;
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
	for (Texture *texture : _texturesLoaded) {
		if (texture != nullptr) {
			delete texture;
		}
	}
}

TextureManager &TextureManager::operator=(TextureManager const &rhs) {
	if (this != &rhs) {
		// clone _blocks array
		std::array<TextureManager::BlockTexture *, 4> const &rhsBlocks = rhs.getBlocks();
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
		_texturesLoaded = std::vector<TextureManager::Texture *>(rhs.getTexturesLoaded());
		for (auto it = _texturesLoaded.begin(); it != _texturesLoaded.end(); ++it) {
			*it = new Texture((*it)->id, (*it)->path);
		}
	}
	return *this;
}

int8_t	TextureManager::loadTextures(std::string const &path) {
	int8_t	res;
	bool skip = false;

	// verify if the texture has been loaded already
	for (u_int32_t i = 0; i < _texturesLoaded.size(); ++i) {
		if (path == _texturesLoaded[i]->path) {
			res = i;
			skip = true;
			break;
		}
	}

	// if not, load it
	if (!skip) {
		TextureManager::Texture	*texture = new TextureManager::Texture();
		bool inSpaceSRGB = true;

		try {
			texture->id = textureFromFile(path, inSpaceSRGB);
		}
		catch(TextureFailToLoad const & e) {
			throw TextureManager::failed2LoadTextureException();
		}

		texture->path = path;
		// save to _texturesLoaded array to skip duplicate textures loading later
		_texturesLoaded.push_back(texture);
		res = _texturesLoaded.size() - 1;
	}

	return res;
}

void	TextureManager::loadBlocksTextures(nlohmann::json const &data) {
	// read json data to fill blocks textures infos
	if (data.find("blocks") != data.end()) {
		for (auto &block : data["blocks"].items()) {
			// valid material name
			auto it = std::find(_blocksNames.begin(), _blocksNames.end(), block.key());
			if (it != _blocksNames.end()) {
				int8_t id = it - _blocksNames.begin();

				// texture has not already been defined
				if (_blocks[id] == nullptr) {
					BlockTexture	*blockTexture = new BlockTexture();

					// fill blockTexture infos
					for (auto &blockText : block.value().items()) {
						std::string	key = blockText.key();
						if (key == "default") {
							blockTexture->side = loadTextures(blockText.value());
						}
						else if (key == "top") {
							blockTexture->top = loadTextures(blockText.value());
						}
						else if (key == "bottom") {
							blockTexture->bottom = loadTextures(blockText.value());
						}
						else {
							std::cout << "WARNING ! block \"" << _blocksNames[id] << \
							"\", unrecognized texture key \"" <<  key  << '"' << std::endl;
						}
					}

					if (blockTexture->side == -1) {
						std::cerr << "missing default texture for block \"" << \
						_blocksNames[id] << '"' << std::endl;
						throw TextureManager::missingBlockException();
					}

					_blocks[id] = blockTexture;
				}
			}
		}
	}

	// check for missing block textures info
	bool missingBlock = false;
	for (size_t i = 0; i < _blocks.size(); ++i) {
		if (_blocks[i] == nullptr) {
			missingBlock = true;
			std::cerr << "missing block: " << _blocksNames[i] << std::endl;
		}
	}
	if (missingBlock) {
		throw TextureManager::missingBlockException();
	}
}

void	TextureManager::setUniform(Shader &sh) const {
	// activate textures
	for (size_t i = 0; i < _texturesLoaded.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		sh.setInt("blockTextures[" + std::to_string(i) + "]", i);
		glBindTexture(GL_TEXTURE_2D, _texturesLoaded[i]->id);
	}
	glActiveTexture(GL_TEXTURE0);

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
		if (_blocks[i]->top != -1) {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureBottom", _blocks[i]->bottom);
		}
		else {
			sh.setInt("blockTexturesInfo[" + std::to_string(i) + "].textureBottom", _blocks[i]->side);
		}
	}
}

std::vector<TextureManager::Texture *> const &	TextureManager::getTexturesLoaded() const {
	return _texturesLoaded;
}
std::array<TextureManager::BlockTexture *, 4> const &	TextureManager::getBlocks() const {
	return _blocks;
}

std::ostream & operator << (std::ostream &out, const TextureManager::Texture &m) {
	out << "{ id: " << m.id << ", path: " << m.path << " }";
	return out;
}

std::ostream & operator << (std::ostream &out, const TextureManager &tm) {
	std::vector<TextureManager::Texture *> const &texturesLoaded = tm.getTexturesLoaded();
	std::array<TextureManager::BlockTexture *, 4> const &blocks = tm.getBlocks();

	for (TextureManager::BlockTexture *b : blocks) {
		if (b != nullptr) {
			out << "{ " << std::endl;
			if (b->side != -1) {
				out << " side: " << b->side << ": " << *(texturesLoaded[b->side]) << std::endl;
			}
			if (b->top != -1) {
				out << " top: " << b->top << ": " << *(texturesLoaded[b->top]) << std::endl;
			}
			if (b->bottom != -1) {
				out << " bottom: " << b->bottom << ": " << *(texturesLoaded[b->bottom]) << std::endl;
			}
			out << "}" << std::endl;
		}
	}

	return out;
}
