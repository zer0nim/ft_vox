#include <iostream>
#include <boost/filesystem.hpp>
#include "ft_vox.hpp"

bool	createDir(std::string const &dirNames) {
	if (boost::filesystem::is_directory(dirNames) == false) {
		try {
			if (boost::filesystem::create_directories(dirNames) == false) {
				logErr("failed to create " << dirNames);
				return false;
			}
		}
		catch (boost::filesystem::filesystem_error &e) {
			logErr("failed to create " << dirNames << " -> " << e.what());
			return false;
		}
	}
	return true;
}
bool	createDir(char const *dirNames) { return createDir(std::string(dirNames)); }

bool	createMapFiles(std::string const &mapName) {
	// create the maps directory
	if (createDir(s.g.files.mapsPath) == false) {
		return false;
	}

	// create map (is needed)
	if (createDir(mapName) == false) {
		return false;
	}

	// create map (is needed)
	if (createDir(mapName + "/" + s.g.files.chunkPath) == false) {
		return false;
	}
	return true;
}
