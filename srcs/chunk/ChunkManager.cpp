#include "ChunkManager.hpp"

ChunkManager::ChunkManager() {
}

ChunkManager::ChunkManager(ChunkManager const &src) {
	*this = src;
}

ChunkManager::~ChunkManager() {
}

ChunkManager &ChunkManager::operator=(ChunkManager const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}
