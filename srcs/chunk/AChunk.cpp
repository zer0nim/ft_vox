#include <fstream>
#include <sstream>
#include <limits>
#include <boost/filesystem.hpp>
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"

AChunk * instanciateNewChunk() {
	AChunk * newChunk = new Chunk;
	return newChunk;
}

AChunk::AChunk() :
_data(),
_filename(""),
_isModifiedFromBegining(false) {
}

AChunk::AChunk(AChunk const &src) {
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

void	AChunk::createChunk(std::string const &mapName, wordIVec3 const &chunkPos) {
	createChunk(mapName, vecToString(chunkPos));
}
void	AChunk::createChunk(std::string const &mapName, std::string const &chunkPos) {
	bool normalLoad = true;
	_filename = mapName + "/" + CHUNK_PATH + chunkPos + CHUNK_EXTENSION;
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
