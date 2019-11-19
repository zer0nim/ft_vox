#include <unistd.h>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "ft_vox.hpp"
#include "TextureManager.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "utils/Shader.hpp"
#include "utils/Skybox.hpp"
#include "utils/TextRender.hpp"

std::chrono::milliseconds getMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
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

void	gameLoop(GLFWwindow *window, Camera const &cam, Skybox &skybox, \
TextRender &textRender, ChunkManager &chunkManager) {
	std::chrono::milliseconds	time_start;
	int							lastFps;
	tWinUser	*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	bool firstLoop = true;

	wordFVec3 pos(0, 0, 0);

	// projection matrix
	glm::mat4	projection = glm::perspective(
		glm::radians(cam.zoom), winU->width / winU->height, 0.1f, 100.0f);

	chunkManager.init(winU->cam->pos, projection);

	skybox.getShader().use();
	skybox.getShader().setMat4("projection", projection);

	std::cout << "init done" << std::endl;

	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
	checkError();
	while (!glfwWindowShouldClose(window)) {
		time_start = getMs();

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view matrix
		glm::mat4	view = cam.getViewMatrix();

		glm::mat4	skyView = view;
		skyView[3][0] = 0;  // remove translation for the skybox
		skyView[3][1] = 0;
		skyView[3][2] = 0;
		skybox.getShader().use();
		skybox.getShader().setMat4("view", skyView);

		// draw here
		chunkManager.update(winU->cam->pos);
		chunkManager.draw(view);

		// draw skybox
		skybox.draw();

		// draw text
		drawText(window, textRender, lastFps, chunkManager);

		glfwSwapBuffers(window);
		glfwPollEvents();
		checkError();

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > LOOP_TIME) {
			lastFps = static_cast<int>(1000.0f / time_loop.count());
			#if DEBUG == true
				if (!firstLoop)
					std::cerr << "loop slow -> " << time_loop.count() << "ms / " << LOOP_TIME << "ms (" << FPS << "fps)\n";
			#endif
		}
		else {
			lastFps = FPS;
			usleep((LOOP_TIME - time_loop.count()) * 1000);
		}
		firstLoop = false;
	}
}

bool	init(GLFWwindow **window, const char *name, tWinUser *winU, Camera *cam) {
	winU->cam = cam;
	winU->dtTime = 0.0f;
	winU->lastFrame = 0.0f;
	winU->width = SCREEN_W;
	winU->height = SCREEN_H;
	winU->showInfo = true;

	if (!initWindow(window, name, winU))
		return (false);
	return (true);
}

int		main(int ac, char const **av) {
	GLFWwindow		*window;
	tWinUser		winU;
	Camera			cam(glm::vec3(0.0f, 64.0f, 19.0f));
	TextureManager	*textureManager = nullptr;

	if (ac != 2) {
		std::cout << "Usage: ./ft_vox <map_name>" << std::endl;
		return 0;
	}
	std::string mapName = std::string(MAPS_PATH) + av[1];
	if (createMapFiles(mapName) == false) {
		return 1;
	}

	if (!init(&window, "ft_vox", &winU, &cam))
		return (1);

	try {
		// load textures
		textureManager = new TextureManager("./assets/textures.json");

		// load all shaders
		Shader textShader("./shaders/text_vs.glsl", "./shaders/text_fs.glsl");
		Shader skyboxShader("./shaders/skybox_vs.glsl", "./shaders/skybox_fs.glsl");

		// load all fonts
		TextRender textRender(textShader);
		textRender.loadFont("title", "fonts/minecraft_title.ttf", TEXT_SIZE_TITLE);
		textRender.loadFont("normal", "fonts/minecraft_normal.ttf", TEXT_SIZE_NORMAL);

		// load skybox
		Skybox skybox(skyboxShader);

		// create chunkManager
		ChunkManager chunkManager(mapName, *textureManager);

		// run the game
		gameLoop(window, cam, skybox, textRender, chunkManager);
	}
	catch(const TextureManager::TextureManagerError& e) {
		std::cerr << e.what() << std::endl;

		glfwDestroyWindow(window);
		glfwPollEvents();
		glfwTerminate();
		return 1;
	}
	catch(const Shader::ShaderError& e) {
		std::cout << "ShaderError !" << std::endl;
		std::cerr << e.what() << std::endl;
	}
	// TODO(tnicolas42): add textRender errors catchings

	delete textureManager;
	glfwDestroyWindow(window);
	glfwPollEvents();
	glfwTerminate();

	return 0;
}
