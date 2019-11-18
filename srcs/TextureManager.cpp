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
			drawBlocks();
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
			TextureManager::Texture	*tmp = *it;
			*it = new Texture((*it)->id, (*it)->path);

			for (size_t i = 0; i < rhsBlocks.size(); ++i) {
				if (_blocks[i] != nullptr) {
					if (_blocks[i]->side == tmp) {
						_blocks[i]->side = *it;
					}
					if (_blocks[i]->top == tmp) {
						_blocks[i]->top = *it;
					}
					if (_blocks[i]->bottom == tmp) {
						_blocks[i]->bottom = *it;
					}
				}
			}
		}
	}
	return *this;
}

void	TextureManager::drawBlocks() const {
	for (BlockTexture *block : _blocks) {
		if (block != nullptr) {
			std::cout << *block;
		}
	}
}


TextureManager::Texture	*TextureManager::loadTextures(std::string const &path) {
	TextureManager::Texture	*res;
	bool skip = false;

	// verify if the texture has been loaded already
	for (u_int32_t i = 0; i < _texturesLoaded.size(); ++i) {
		if (path == _texturesLoaded[i]->path) {
			res = _texturesLoaded[i];
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
		res = texture;
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

					if (blockTexture->side == nullptr) {
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

std::ostream & operator << (std::ostream &out, const TextureManager::BlockTexture &m) {
	out << "{ " << std::endl;
	if (m.side != nullptr) {
		out << " side: " << m.side << ": " << *(m.side) << std::endl;
	}
	if (m.top != nullptr) {
		out << " top: " << m.top << ": " << *(m.top) << std::endl;
	}
	if (m.bottom != nullptr) {
		out << " bottom: " << m.bottom << ": " << *(m.bottom) << std::endl;
	}
	out << "}" << std::endl;
	return out;
}
