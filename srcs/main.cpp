#include <unistd.h>
#include <boost/filesystem.hpp>

#include "ft_vox.hpp"
#include "TextureManager.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "utils/Shader.hpp"
#include "utils/Skybox.hpp"

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

void	gameLoop(GLFWwindow *window, Camera const &cam, Shader &skyboxSh, Skybox &skybox, \
AChunk &chunk) {
	std::chrono::milliseconds time_start;
	tWinUser	*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	bool firstLoop = true;

	wordFVec3 pos(0, 0, 0);

	// projection matrix
	glm::mat4	projection = glm::perspective(
		glm::radians(cam.zoom), winU->width / winU->height, 0.1f, 100.0f);

	skyboxSh.use();
	skyboxSh.setMat4("projection", projection);

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
		skyboxSh.use();
		skyboxSh.setMat4("view", skyView);

		// draw here

		chunk.update();
		chunk.draw();

		skybox.draw();  // draw shader

		glfwSwapBuffers(window);
		glfwPollEvents();
		checkError();

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

bool	init(GLFWwindow **window, const char *name, tWinUser *winU, Camera *cam) {
	winU->cam = cam;
	winU->dtTime = 0.0f;
	winU->lastFrame = 0.0f;
	winU->width = SCREEN_W;
	winU->height = SCREEN_H;

	if (!initWindow(window, name, winU))
		return (false);
	return (true);
}

int		main(int ac, char const **av) {
	GLFWwindow	*window;
	tWinUser	winU;
	Camera		cam(glm::vec3(0.0f, 0.0f, 3.0f));
	AChunk		*chunk;
	TextureManager	*textureManager;

	(void)ac;
	(void)av;
	// if (ac != 2) {
	// 	std::cout << "Usage: ./ft_vox <map_name>" << std::endl;
	// 	return 0;
	// }
	// std::string mapName = std::string(MAPS_PATH) + av[1];
	// if (createMapFiles(mapName) == false) {
	// 	return 1;
	// }

	// ChunkManager * chunkManager = new ChunkManager(mapName);

	// chunkManager->init(wordFVec3(0, 0, 0));

	if (!init(&window, "ft_vox", &winU, &cam))
		return (1);

	try {
		textureManager = new TextureManager("./assets/textures.json");

		Shader skyboxShader("./shaders/skybox_vs.glsl", "./shaders/skybox_fs.glsl");
		Skybox skybox(skyboxShader);

		chunk = new Chunk;
		chunk->oldCreateChunk();
		chunk->updateBlock(chunkVec3(0, 0, 0), 1);
		chunk->updateBlock(chunkVec3(15, 0, 0), 1);
		chunk->updateBlock(chunkVec3(15, 0, 15), 1);
		chunk->updateBlock(chunkVec3(0, 0, 15), 1);
		chunk->updateBlock(chunkVec3(0, 15, 0), 1);
		chunk->updateBlock(chunkVec3(15, 15, 0), 1);
		chunk->updateBlock(chunkVec3(15, 15, 15), 1);
		chunk->updateBlock(chunkVec3(0, 15, 15), 1);

		gameLoop(window, cam, skyboxShader, skybox, *chunk);

		delete chunk;
	}
	catch(const TextureManager::TextureManagerError& e) {
		std::cerr << e.what() << std::endl;

		// delete chunkManager;
		glfwDestroyWindow(window);
		glfwPollEvents();
		glfwTerminate();
		return 1;
	}
	catch(const Shader::ShaderError& e) {
		std::cout << "ShaderError !" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	delete textureManager;
	// delete chunkManager;
	glfwDestroyWindow(window);
	glfwPollEvents();
	glfwTerminate();

	return 0;
}
