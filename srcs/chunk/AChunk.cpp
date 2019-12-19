#include <fstream>
#include <sstream>
#include <limits>
#include <boost/filesystem.hpp>
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "GreedyChunk.hpp"
#include "GreedyChunk2.hpp"
#include "GreedyChunk3.hpp"
#include "ChunkManager.hpp"
#include "MapGenerator.hpp"

std::unique_ptr<AChunk::ShaderData>	AChunk::_shaderData = \
	std::unique_ptr<AChunk::ShaderData>();

AChunk * instanciateNewChunk(TextureManager const &textureManager) {
	AChunk * newChunk = new CHUNK_OBJECT(textureManager);
	return newChunk;
}

AChunk::AChunk(TextureManager const &textureManager)
: _data(),
  _filename(""),
  _isModifiedFromBegining(false),
  _textureManager(textureManager) {}

AChunk::AChunk(AChunk const &src)
: _textureManager(src.getTextureManager()) {
	*this = src;
}

AChunk::~AChunk() {}

AChunk &AChunk::operator=(AChunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void	AChunk::draw(glm::mat4 &view, glm::vec3 &pos) {
    { std::lock_guard<std::mutex>	guard(mutexChunk);
		_draw(view, pos);
	}
}

bool	AChunk::_createChunkFromFile() {
	std::ifstream chunkFile(_filename);
	if (chunkFile.fail()) {
		logErr("failed to open file " << _filename << " " << strerror(errno));
		return 1;
	}
	std::string line;
	std::getline(chunkFile, line);
	if (!chunkFile) {
		if (!chunkFile.eof()) {
			logErr("corrupted file: " << _filename << " empty file");
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
						logErr("corrupted file: " << _filename << " not enouth values");
						return false;
					}
				}
				number = std::atoi(strNumber.c_str());
				if (number < 0 || number > std::numeric_limits<uint8_t>::max()
				|| (number == 0 && strNumber != "0")) {
						logErr("corrupted file: " << _filename << " invalid value " << strNumber);
					return false;
				}
				_data.data[x][y][z] = number;
			}
		}
	}
	return true;
}

void	AChunk::_createChunk() {
	getChunk(_chunkPos, _data.data);
}

void	AChunk::createChunk(wordIVec3 const &chunkPos) {
	bool normalLoad = true;

	_chunkPos = chunkPos;
	if (s.m.mapName != "") {  // if there is a map
		_filename = s.m.fullMapName + "/" + s.g.files.chunkPath + glm::to_string(chunkPos) + CHUNK_EXTENSION;
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
		logWarn("unable to save chunk: " << _filename << " " << strerror(errno));
		return;
	}
	chunkFile << fileData << std::endl;
	if (chunkFile.fail()) {
		logWarn("unable to save chunk: " << _filename << " " << strerror(errno));
		return;
	}
	chunkFile.close();
}

AChunk::ChunkData const	&AChunk::getData() const { return _data; }
TextureManager const	&AChunk::getTextureManager() const { return _textureManager; }
bool					AChunk::isModifiedFromBegining() const { return _isModifiedFromBegining; }
