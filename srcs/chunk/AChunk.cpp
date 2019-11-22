#include <fstream>
#include <sstream>
#include <limits>
#include <boost/filesystem.hpp>
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "GreedyChunk.hpp"
#include "ChunkManager.hpp"
#include "MapGenerator.hpp"

AChunk * instanciateNewChunk(TextureManager const &textureManager, glm::mat4 &projection) {
	AChunk * newChunk = new CHUNK_OBJECT(textureManager, projection);
	return newChunk;
}

AChunk::AChunk(TextureManager const &textureManager, glm::mat4 &projection) :
isDrawing(false),
isUpdating(false),
_data(),
_filename(""),
_isModifiedFromBegining(false),
_textureManager(textureManager) {
	(void)projection;
}

AChunk::AChunk(AChunk const &src)
: _textureManager(src.getTextureManager()) {
	*this = src;
}

AChunk::~AChunk() {
	if (s.g.files.saveAllChunks || _isModifiedFromBegining) {  // if we need to save
		save();
	}
}

AChunk &AChunk::operator=(AChunk const &rhs) {
	if (this != &rhs) {
		isUpdating = false;
		isDrawing = false;
	}
	return *this;
}

void	AChunk::draw(glm::mat4 &view) const {
	while (isUpdating) {
		usleep(10);
	}
	isDrawing = true;
	_draw(view);
	isDrawing = false;
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
	for (uint8_t ix = 0; ix < CHUNK_SZ_X; ix++) {
		for (uint8_t iy = 0; iy < CHUNK_SZ_Y; iy++) {
			for (uint8_t iz = 0; iz < CHUNK_SZ_Z; iz++) {
				_data.data[ix][iy][iz] = getBlock(_chunkPos, ix, iy, iz);
			}
		}
	}
}

void	AChunk::createChunk(std::string const &chunkPos) {
	createChunk(stringToVec(chunkPos));
}
void	AChunk::createChunk(wordIVec3 const &chunkPos) {
	bool normalLoad = true;

	_chunkPos = chunkPos;
	if (s.m.mapName != "") {  // if there is a map
		_filename = s.m.fullMapName + "/" + s.g.files.chunkPath + vecToString(chunkPos) + CHUNK_EXTENSION;
		if (boost::filesystem::is_regular_file(_filename)) {
			normalLoad = false;
			if (_createChunkFromFile() == false) {
				normalLoad = true;
			}
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
		std::cout << "unable to save chunk: " << _filename << " " << strerror(errno) << std::endl;
		return;
	}
	chunkFile << fileData << std::endl;
	if (chunkFile.fail()) {
		std::cout << "unable to save chunk: " << _filename << " " << strerror(errno) << std::endl;
		return;
	}
	chunkFile.close();
}

AChunk::ChunkData const	&AChunk::getData() const { return _data; }
TextureManager const &AChunk::getTextureManager() const { return _textureManager; }
