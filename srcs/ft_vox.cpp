#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "ft_vox.hpp"
#include "ChunkManager.hpp"
#include "TextRender.hpp"

Settings s;

void	setDefaultSettings() {
	s.g.renderDist = 8;
	s.g.files.mapsPath = "/tmp/ft_vox/maps/";
	s.g.files.chunkPath = "chunks/";
	s.g.files.mapSettingsPath = "map_settings.json";
	s.g.files.saveAllChunks = false;
	s.g.screen.fullscreen = true;
	s.g.screen.width = 1200;
	s.g.screen.height = 800;
	s.g.screen.fps = 120;
	s.g.screen.text.insert(std::pair<std::string, Settings::Global::Screen::Text>(
		"normal", {"assets/fonts/minecraft_normal.ttf", 20}));
	s.g.screen.text.insert(std::pair<std::string, Settings::Global::Screen::Text>(
		"title", {"assets/fonts/minecraft_title.ttf", 24}));
	s.g.screen.text.insert(std::pair<std::string, Settings::Global::Screen::Text>(
		"courrier_new", {"assets/fonts/courrier_new.ttf", 40}));
	uint32_t seedRand = time(nullptr);
	s.m.seed = rand_r(&seedRand);
	s.m.generationType = GENERATION_NORMAL;
	s.m.cameraStartPos.pos.x = 0;
	s.m.cameraStartPos.pos.y = 64;
	s.m.cameraStartPos.pos.z = 0;
	s.m.cameraStartPos.yaw = -90;
	s.m.cameraStartPos.pitch = 0;
	s.m.mapName = "";
	s.m.flatMap.push_back({0, 0, "bedrock"});
	s.m.flatMap.push_back({1, 2, "stone"});
	s.m.flatMap.push_back({3, 3, "grass"});
}

static void	loadSettingElementFont(nlohmann::json &element, std::string &key) {
	static std::string textFont = ".global.screen.text.";
	if (element.is_string() && boost::ends_with(key, ".path")) {
		std::string name = key.substr(textFont.size(), key.size() - textFont.size() - std::string(".path").size());
		if (s.g.screen.text.find(name) == s.g.screen.text.end())
			std::cout << "[WARN]: invalid font name: " << name << std::endl;
		else
			s.g.screen.text[name].path = element.get<std::string>();
	}
	else if (element.is_number() && boost::ends_with(key, ".size")) {
		std::string name = key.substr(textFont.size(), key.size() - textFont.size() - std::string(".size").size());
		if (s.g.screen.text.find(name) == s.g.screen.text.end())
			std::cout << "[WARN]: invalid font name: " << name << std::endl;
		else
			s.g.screen.text[name].size = element.get<uint32_t>();
	}
	else
		std::cout << "[WARN]: invalid argument or argument type in settings: " << key << ": " << element << std::endl;
}

static void	loadFlatMap(nlohmann::json &element) {
	for (auto it = element.begin(); it != element.end(); ++it) {
		try {
			Settings::Map::FlatMap flatMapElem;
			flatMapElem.fromY = it->at("fromY").get<uint32_t>();
			flatMapElem.toY = it->at("toY").get<uint32_t>();
			flatMapElem.blockName = it->at("blockName").get<std::string>();
			s.m.flatMap.push_back(flatMapElem);
		}
		catch (std::exception & e) {
			std::cerr << "[WARN]: flatMap need fromY, toY and blockName keys";
			return;
		}
	}
}

static void	loadSettingElement(nlohmann::json &element, std::string key) {
	if (element.is_number() && key == ".global.renderDist")
		s.g.renderDist = element.get<uint32_t>();
	else if (element.is_string() && key == ".global.files.mapsPath")
		s.g.files.mapsPath = element.get<std::string>();
	else if (element.is_string() && key == ".global.files.chunkPath")
		s.g.files.chunkPath = element.get<std::string>();
	else if (element.is_string() && key == ".global.files.mapSettingsPath")
		s.g.files.mapSettingsPath = element.get<std::string>();
	else if (element.is_boolean() && key == ".global.files.saveAllChunks")
		s.g.files.saveAllChunks = element.get<bool>();
	else if (element.is_boolean() && key == ".global.screen.fullscreen")
		s.g.screen.fullscreen = element.get<bool>();
	else if (element.is_number() && key == ".global.screen.width")
		s.g.screen.width = element.get<uint32_t>();
	else if (element.is_number() && key == ".global.screen.height")
		s.g.screen.height = element.get<uint32_t>();
	else if (element.is_number() && key == ".global.screen.fps")
		s.g.screen.fps = element.get<uint32_t>();
	else if (boost::starts_with(key, ".global.screen.text."))
		loadSettingElementFont(element, key);
	else if (element.is_number() && key == ".map.generationType")
		s.m.generationType = element.get<uint32_t>();
	else if (element.is_number() && key == ".map.cameraStartPos.pos.x")
		s.m.cameraStartPos.pos.x = element.get<float>();
	else if (element.is_number() && key == ".map.cameraStartPos.pos.y")
		s.m.cameraStartPos.pos.y = element.get<float>();
	else if (element.is_number() && key == ".map.cameraStartPos.pos.z")
		s.m.cameraStartPos.pos.z = element.get<float>();
	else if (element.is_number() && key == ".map.cameraStartPos.yaw")
		s.m.cameraStartPos.yaw = element.get<float>();
	else if (element.is_number() && key == ".map.cameraStartPos.pitch")
		s.m.cameraStartPos.pitch = element.get<float>();
	else if (element.is_number() && key == ".map.seed")
		s.m.seed = element.get<uint32_t>();
	else if (element.is_array() && key == ".map.flatMap")
		loadFlatMap(element);
	else
		std::cout << "[WARN]: invalid argument or argument type in settings: " << key << ": " << element << std::endl;
}

static void	loadSettingsJson(nlohmann::json &data, std::string startKey = "") {
	bool	hasFlatMap = false;
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (it->is_object()) {
			loadSettingsJson(*it, startKey + "." + it.key());
		}
		else {
			std::string key = startKey + "." + it.key();
			if (hasFlatMap == false && key == ".map.flatMap") {
				hasFlatMap = true;
				s.m.flatMap.clear();
			}
			loadSettingElement(*it, key);
		}
	}
}

void	loadSettings(std::string settingFile) {
	try {
		std::ifstream fileStream(settingFile, std::ifstream::in);

		if (fileStream.is_open()) {
			nlohmann::json	data;
			data << fileStream;
			loadSettingsJson(data);
		}
		else {
			std::cout << "throw FailedToOpenException" << std::endl;
			throw Settings::FailedToOpenException(std::string(settingFile));
		}
	}
	catch (const nlohmann::json::parse_error& e) {
		std::cerr << "message: " << e.what() << std::endl;
		std::cerr << "exception id: " << e.id << std::endl;
		std::cerr << "byte position of error: " << e.byte << std::endl;
		throw Settings::JsonParseException();
	}
	catch (std::exception &e) {
		std::cerr << "[ERROR]: unexpected exception in json loading" << std::endl;
		throw Settings::JsonParseException();
	}
}

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

bool	argparse(int nbArgs, char const **args) {
	int i = 0;
	while (i < nbArgs) {
		if (strcmp(args[i], "--usage") == 0 || strcmp(args[i], "-u") == 0) {
			return usage();
		}
		else if (strcmp(args[i], "--name") == 0 || strcmp(args[i], "-n") == 0) {
			i++;
			if (i == nbArgs || args[i][0] == '-')
				return usage();
			s.m.mapName = args[i];
		}
		else if (strcmp(args[i], "--seed") == 0 || strcmp(args[i], "-s") == 0) {
			i++;
			if (i == nbArgs)
				return usage();
			s.m.seed = static_cast<uint32_t>(atoi(args[i]));
			if (s.m.seed == 0)
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

bool	createMapFiles() {
	// create the maps directory
	if (createDir(s.g.files.mapsPath) == false) {
		return false;
	}

	s.m.fullMapName = std::string(s.g.files.mapsPath) + s.m.mapName;
	if (boost::filesystem::is_directory(s.m.fullMapName) == false)
		std::cout << "[INFO]: create " << s.m.mapName << std::endl;
	else
		std::cout << "[INFO]: load " << s.m.mapName << std::endl;
	// create map (if needed)
	if (createDir(s.m.fullMapName) == false) {
		return false;
	}

	// create map (if needed)
	if (createDir(s.m.fullMapName + "/" + s.g.files.chunkPath) == false) {
		return false;
	}

	// check if the settings file exist
	std::string settingsFilename = s.m.fullMapName + "/" + s.g.files.mapSettingsPath;
	if (boost::filesystem::is_regular_file(settingsFilename) == true) {
		// load settings
		try {
			loadSettings(settingsFilename);
		}
		catch (Settings::SettingsError &e) {
			std::cout << "[ERROR]: unable to load settings from map" << std::endl;
			return false;
		}
	}
	else if (boost::filesystem::is_directory(s.m.fullMapName) == true) {
		std::cout << "[WARN]: unable to load settings from map" << std::endl;
	}
	return true;
}

bool	saveMap(Camera &cam) {
	std::string		settingsFilename = s.m.fullMapName + "/" + s.g.files.mapSettingsPath;
	nlohmann::json	lastSettings;
	try {
		std::ifstream fileStream(settingsFilename, std::ifstream::in);
		if (fileStream.is_open()) {
			lastSettings << fileStream;
		}
	}
	catch (const nlohmann::json::parse_error& e) {}
	catch (std::exception &e) {}
	// create flatMapFiles
	std::vector<nlohmann::json> flatMapElem;
	for (auto it = s.m.flatMap.begin(); it != s.m.flatMap.end(); it++) {
		flatMapElem.push_back({{"fromY", it->fromY}, {"toY", it->toY}, {"blockName", it->blockName}});
	}
	nlohmann::json	settings = {
		{"map", {
			{"seed", s.m.seed},
			{"generationType", s.m.generationType},
			{"cameraStartPos", {
				{"pos", {
					{"x", cam.pos.x},
					{"y", cam.pos.y},
					{"z", cam.pos.z}
				}},
			    {"yaw", cam.yaw},
			    {"pitch", cam.pitch}
			}},
			{"flatMap", flatMapElem}
		}
	}};
	try {
		lastSettings.erase("flatMap");
	}
	catch (nlohmann::detail::type_error & e) {}
	lastSettings.merge_patch(settings);
	std::ofstream settingsFile(settingsFilename);
	if (settingsFile.fail()) {
		std::cout << "unable to save map settings: " << settingsFilename << " " << strerror(errno) << std::endl;
		return false;
	}
	settingsFile << std::setw(4) << lastSettings << std::endl;
	if (settingsFile.fail()) {
		std::cout << "unable to save map settigns: " << settingsFilename << " " << strerror(errno) << std::endl;
		return false;
	}
	settingsFile.close();
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

	GLfloat lineSz = s.g.screen.text["normal"].size * 1.2;
	GLfloat textX = 10;
	GLfloat textY = s.g.screen.height - lineSz;

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
	std::string sPosChunk;
    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
		sPosChunk = std::string("Chunk: ") + std::to_string(chunkManager.getChunkActPos().x) + " "
			+ std::to_string(chunkManager.getChunkActPos().y) + " " +std::to_string(chunkManager.getChunkActPos().z)
			+ " (" + std::to_string(chunkManager.getNbChunkRendered()) + " chunks rendered on "
			+ std::to_string(chunkManager.getNbChunkLoaded()) + " loaded)";
	}
	textRender.write("normal", sPosChunk, textX, textY);

	if (winU->freezeChunkUpdate) {
		// freeze chunk update
		textY -= lineSz;
		std::string sFreeze = "Chunk update: freeze (unfreeze with F3 + F)";
		textRender.write("normal", sFreeze, textX, textY, 1, glm::vec4(1, 0, 0, 1));
	}

	if (winU->showHelp) {
		// show help
		textY -= lineSz;
		std::string sHelp = "Help:";
		textRender.write("normal", sHelp, textX, textY);
		textY -= lineSz;
		sHelp = "F3 + F: freeze chunk update";
		textRender.write("normal", sHelp, textX + 20, textY);
		textY -= lineSz;
		sHelp = "F3 + H: toggle help menu";
		textRender.write("normal", sHelp, textX + 20, textY);
		textY -= lineSz;
		sHelp = "F3 + P: toggle polygon render mode";
		textRender.write("normal", sHelp, textX + 20, textY);
	}
}
