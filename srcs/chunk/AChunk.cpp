#include <fstream>
#include <sstream>
#include <limits>
#include <boost/filesystem.hpp>
#include "AChunk.hpp"
#include "GreedyChunk3.hpp"
#include "ChunkManager.hpp"
#include "MapGenerator.hpp"

std::unique_ptr<AChunk::ShaderData>	AChunk::_shaderData = \
	std::unique_ptr<AChunk::ShaderData>();

AChunk * instanciateNewChunk(TextureManager const &textureManager, ChunkManager &chunkManager) {
	AChunk * newChunk = new CHUNK_OBJECT(textureManager, chunkManager);
	return newChunk;
}

AChunk::AChunk(TextureManager const &textureManager, ChunkManager &chunkManager)
:  _data(),
  _filename(""),
  _isModifiedFromBegining(false),
  _needRenderUpdate(false),
  _textureManager(textureManager),
  _chunkManager(chunkManager) {}

AChunk::AChunk(AChunk const &src)
: _textureManager(src.getTextureManager()),
  _chunkManager(src.getChunkManager()) {
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
				if (number > NB_TYPE_BLOCKS) {
					logWarn("invalid block (" << number << "). replaced by bedrock");
					_data.data[x][y][z] = TextureManager::blocksNames["bedrock"];
				}
				else {
					_data.data[x][y][z] = number;
				}
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

	// update chunk around if needed
	wordIVec3 tmpChunkPos;
	if (pos.x == 0) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.x -= CHUNK_SZ_X;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}
	else if (pos.x == CHUNK_SZ_X - 1) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.x += CHUNK_SZ_X;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}
	if (pos.y == 0) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.y -= CHUNK_SZ_Y;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}
	else if (pos.y == CHUNK_SZ_Y - 1) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.y += CHUNK_SZ_Y;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}
	if (pos.z == 0) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.z -= CHUNK_SZ_Z;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}
	else if (pos.z == CHUNK_SZ_Z - 1) {
		tmpChunkPos = _chunkPos;
		tmpChunkPos.z += CHUNK_SZ_Z;
		if (_chunkManager.isChunkExist(tmpChunkPos)) {
			_chunkManager.getChunkMap()[tmpChunkPos]->getData().isModified = true;
		}
	}

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

Shader					&AChunk::getShader() { return *(_shaderData->shader); }
AChunk::ChunkData const	&AChunk::getData() const { return _data; }
AChunk::ChunkData		&AChunk::getData() { return _data; }
TextureManager const	&AChunk::getTextureManager() const { return _textureManager; }
ChunkManager			&AChunk::getChunkManager() const { return _chunkManager; }
bool					AChunk::isModifiedFromBegining() const { return _isModifiedFromBegining; }
