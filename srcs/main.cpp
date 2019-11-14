#include <unistd.h>
#include <chrono>
#include <boost/filesystem.hpp>
#include "ft_vox.hpp"
#include "commonInclude.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

bool	createDir(std::string const &dirNames) {
	if (boost::filesystem::is_directory(dirNames) == false) {
		try {
			if (boost::filesystem::create_directories(dirNames) == false) {
				std::cout << "failed to create " << dirNames << std::endl;
				return false;
			}
		}
		catch (boost::filesystem::filesystem_error &e) {
			std::cout << "failed to create " << dirNames << " -> " << e.what() << std::endl;
			return false;
		}
	}
	return true;
}
bool	createDir(char const *dirNames) { return createDir(std::string(dirNames)); }

bool	createMapFiles(std::string const &mapName) {
	// create the maps directory
	if (createDir(MAPS_PATH) == false) {
		return false;
	}

	// create map (is needed)
	if (createDir(mapName) == false) {
		return false;
	}

	// create map (is needed)
	if (createDir(mapName + "/" + CHUNK_PATH) == false) {
		return false;
	}
	return true;
}

void	gameLoop(ChunkManager *chunkManager) {
	std::chrono::milliseconds time_start;
	bool firstLoop = true;

	wordFVec3 pos(0, 0, 0);

	while (true/*!glfwWindowShouldClose(window)*/) {
		time_start = getMs();

		chunkManager->update(pos);  // camera pos

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > LOOP_TIME) {
			#if DEBUG == true
				if (!firstLoop)
					std::cerr << "loop slow -> " << time_loop.count() << "ms / " << LOOP_TIME << "ms (" << FPS << "fps)\n";
			#endif
		}
		else {
			usleep((LOOP_TIME - time_loop.count()) * 1000);
		}
		firstLoop = false;
	}
}

int		main(int ac, char **av) {
	if (ac != 2) {
		std::cout << "Usage: ./ft_vox <map_name>" << std::endl;
		return 1;
	}
	std::string mapName = std::string(MAPS_PATH) + av[1];
	if (createMapFiles(mapName) == false) {
		return 1;
	}

	ChunkManager * chunkManager = new ChunkManager(mapName);

	chunkManager->init(wordFVec3(0, 0, 0));

	gameLoop(chunkManager);

	delete chunkManager;
	return 0;
}
