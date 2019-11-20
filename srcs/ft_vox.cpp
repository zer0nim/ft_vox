#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "ft_vox.hpp"
#include "ChunkManager.hpp"
#include "TextRender.hpp"

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
}

bool	usage() {
	std::cout << "Usage: ./ft_vox [--usage|-u] [--name|-n mapName] [--seed|-s seed]" << std::endl;
	std::cout << "\t--usage|-u: display usage of the program" << std::endl;
	std::cout << "\t--name|-n mapName: set map name" << std::endl;
	std::cout << "\t--seed|-s seed: set seed (seed should be an int)" << std::endl;
	return false;
}

bool	argparse(int nbArgs, char const **args, std::string &mapName, uint32_t *seed) {
	int i = 0;
	while (i < nbArgs) {
		if (strcmp(args[i], "--usage") == 0 || strcmp(args[i], "-u") == 0) {
			return usage();
		}
		else if (strcmp(args[i], "--name") == 0 || strcmp(args[i], "-n") == 0) {
			i++;
			if (i == nbArgs || args[i][0] == '-')
				return usage();
			mapName = args[i];
		}
		else if (strcmp(args[i], "--seed") == 0 || strcmp(args[i], "-s") == 0) {
			i++;
			if (i == nbArgs)
				return usage();
			*seed = static_cast<uint32_t>(atoi(args[i]));
			if (*seed == 0)
				return usage();
		}
		else {
			return usage();
		}

		i++;
	}
	return true;
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

	std::string fullMapName = std::string(MAPS_PATH) + mapName;
	if (boost::filesystem::is_directory(fullMapName) == false)
		std::cout << "[INFO]: create " << mapName << std::endl;
	else
		std::cout << "[INFO]: load " << mapName << std::endl;
	// create map (if needed)
	if (createDir(fullMapName) == false) {
		return false;
	}

	// create map (if needed)
	if (createDir(fullMapName + "/" + CHUNK_PATH) == false) {
		return false;
	}
	return true;
}

int counter = 0;
int savedFps = 0;
void	drawText(GLFWwindow *window, TextRender &textRender, int actFps, ChunkManager &chunkManager) {
	tWinUser	*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));

	if (winU->showInfo == false)
		return;

	if (counter % 10 == 0) {
		savedFps = actFps;
		counter = 0;
	}
	counter++;

	GLfloat lineSz = TEXT_SIZE_NORMAL * 1.2;
	GLfloat textX = 10;
	GLfloat textY = winU->height - lineSz;

	// fps
	std::string sFps = std::to_string(savedFps) + " fps";
	textRender.write("normal", sFps, textX, textY);

	// position
	textY -= lineSz;
	std::string sPos = std::string("XYZ: ");
	std::stringstream pos;
	pos << std::fixed << std::setprecision(3) << winU->cam->pos.x << " / ";
	pos << std::fixed << std::setprecision(3) << winU->cam->pos.y << " / ";
	pos << std::fixed << std::setprecision(3) << winU->cam->pos.z;
	sPos += pos.str();
	textRender.write("normal", sPos, textX, textY);

	// block position
	textY -= lineSz;
	std::string sPosBlock = std::string("Block: ") + std::to_string(static_cast<int>(winU->cam->pos.x)) + " " +
		std::to_string(static_cast<int>(winU->cam->pos.y)) + " " +std::to_string(static_cast<int>(winU->cam->pos.z));
	textRender.write("normal", sPosBlock, textX, textY);

	// actual chunk
	textY -= lineSz;
	std::string sPosChunk = std::string("Chunk: ") + std::to_string(chunkManager.getChunkActPos().x) + " " +
		std::to_string(chunkManager.getChunkActPos().y) + " " +std::to_string(chunkManager.getChunkActPos().z);
	textRender.write("normal", sPosChunk, textX, textY);
}
