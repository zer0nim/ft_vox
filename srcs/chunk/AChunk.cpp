#include <fstream>
#include <sstream>
#include <limits>
#include <boost/filesystem.hpp>
#include <glm/gtc/noise.hpp>
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"

AChunk * instanciateNewChunk(TextureManager const &textureManager) {
	AChunk * newChunk = new Chunk(textureManager);
	return newChunk;
}

AChunk::AChunk(TextureManager const &textureManager) :
_data(),
_filename(""),
_isModifiedFromBegining(false),
_textureManager(textureManager) {
}

AChunk::AChunk(AChunk const &src)
: _textureManager(src.getTextureManager()) {
	*this = src;
}

AChunk::~AChunk() {
	if (SAVE_ALL_CHUNKS || _isModifiedFromBegining) {  // if we need to save
		save();
	}
}

AChunk &AChunk::operator=(AChunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

bool	AChunk::_createChunkFromFile() {
	std::ifstream chunkFile(_filename);
	if (chunkFile.fail()) {
		std::cout << "Error: " << strerror(errno) << std::endl;
		return 1;
	}
	std::string line;
	std::getline(chunkFile, line);
	if (!chunkFile) {
		if (!chunkFile.eof()) {
			std::cout << "corrupted file: " << _filename << " empty file" << std::endl;
			chunkFile.close();
			return false;
		}
	}
	chunkFile.close();

	std::stringstream ss(line);
	std::string strNumber;
	int			number;
	for (uint8_t x = 0; x < CHUNK_SZ_X; x++) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; y++) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; z++) {
				std::getline(ss, strNumber, '|');
				if (!ss) {
					if (ss.eof()) {
						std::cout << "corrupted file: " << _filename << " not enouth values" << std::endl;
						return false;
					}
				}
				number = std::atoi(strNumber.c_str());
				if (number < 0 || number > std::numeric_limits<uint8_t>::max()
				|| (number == 0 && strNumber != "0")) {
					std::cout << "corrupted file: " << _filename << " invalid value " << strNumber << std::endl;
					return false;
				}
				_data.data[x][y][z] = number;
			}
		}
	}
	return true;
}

void	AChunk::_createChunk() {
	float xFactor = 1.0 / (MAX_MAP_SIZE_X - 1);
	float yFactor = 1.0 / (MAX_MAP_SIZE_Y - 1);
	float zFactor = 1.0 / (MAX_MAP_SIZE_Z - 1);

	float x, y, z;
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		x = (_chunkPos.x + ix) * xFactor;
		for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
			y = (_chunkPos.y + iy) * yFactor;
			for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
				z = (_chunkPos.z + iz) * zFactor;

				glm::vec3 v(x, y, z);
				float val = glm::perlin(v);
				if (val > 0.1) {
					if (y < 0.25)
						_data.data[ix][iy][iz] = 1;
					else if (y < 0.5)
						_data.data[ix][iy][iz] = 2;
					else if (y < 0.75)
						_data.data[ix][iy][iz] = 3;
					else
						_data.data[ix][iy][iz] = 4;
				}
				else
					_data.data[ix][iy][iz] = 0;
			}
		}
	}
}

void	AChunk::createChunk(std::string const &mapName, std::string const &chunkPos) {
	createChunk(mapName, stringToVec(chunkPos));
}
void	AChunk::createChunk(std::string const &mapName, wordIVec3 const &chunkPos) {
	bool normalLoad = true;

	_chunkPos = chunkPos;
	_filename = mapName + "/" + CHUNK_PATH + vecToString(chunkPos) + CHUNK_EXTENSION;
	if (boost::filesystem::is_regular_file(_filename)) {
		normalLoad = false;
		if (_createChunkFromFile() == false) {
			normalLoad = true;
		}
	}
	if (normalLoad) {
		_createChunk();
	}
}


void AChunk::updateBlock(chunkVec3 pos, uint8_t value) {
	if (_data.data[pos.x][pos.y][pos.z] == value)
		return;  // unchanged chunk
	_data.data[pos.x][pos.y][pos.z] = value;
	_data.isModified = true;
	_isModifiedFromBegining = true;
}

void AChunk::save() {
	if (_filename == "") {
		std::cout << "unable to save: no filename specified" << std::endl;
		return;
	}
	std::string fileData;
	for (uint8_t x = 0; x < CHUNK_SZ_X; x++) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; y++) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; z++) {
				fileData += std::to_string(_data.data[x][y][z]) + "|";
			}
		}
	}
	fileData.pop_back();  // remove the last |
	std::ofstream chunkFile(_filename);
	if (chunkFile.fail()) {
		std::cout << "unable to save chunk: " << _filename << strerror(errno) << std::endl;
		return;
	}
	chunkFile << fileData << std::endl;
	if (chunkFile.fail()) {
		std::cout << "unable to save chunk: " << _filename << strerror(errno) << std::endl;
		return;
	}
	chunkFile.close();
}

AChunk::ChunkData const	&AChunk::getData() const { return _data; }
TextureManager const &AChunk::getTextureManager() const { return _textureManager; }
