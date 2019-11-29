#include <unistd.h>
#include <pthread.h>
#include <iomanip>

#include "ft_vox.hpp"
#include "TextureManager.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "MapGenerator.hpp"
#include "utils/Shader.hpp"
#include "utils/Skybox.hpp"
#include "utils/TextRender.hpp"

void	*threadUpdateFunction(void *args_) {
	float						loopTime = 1000 / s.g.screen.fps;
	ThreadupdateArgs			*args = reinterpret_cast<ThreadupdateArgs*>(args_);
	std::chrono::milliseconds	time_start;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(args->window));
	bool						firstLoop = true;

	while (!glfwWindowShouldClose(args->window)) {
		time_start = getMs();

		args->deleteLocker.isLocked = args->deleteLocker.ask;

		if (args->deleteLocker.isLocked == false) {  // thread is unlocked
			// update
			if (winU->freezeChunkUpdate == false) {
				args->chunkManager.update(args->camPos);
				if (args->chunkManager.toDelete.size() > 0) {  // auto lock
					args->deleteLocker.ask = true;
					continue;
				}
			}
		}

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > loopTime) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					std::cerr << "update loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)\n";
			#endif
		}
		else {
			usleep((loopTime - time_loop.count()) * 1000);
		}
		firstLoop = false;
	}
	args->quit = true;
	pthread_exit(NULL);
	return nullptr;
}

void	gameLoop(GLFWwindow *window, Camera const &cam, Skybox &skybox, \
TextRender &textRender, ChunkManager &chunkManager) {
	float						loopTime = 1000 / s.g.screen.fps;
	std::chrono::milliseconds	time_start;
	int							lastFps = 0;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	bool						firstLoop = true;
	ThreadupdateArgs			*threadUpdateArgs = new ThreadupdateArgs(window, chunkManager, winU->cam->pos);

	// projection matrix
	float angle = cam.zoom;
	float ratio = static_cast<float>(s.g.screen.width) / s.g.screen.height;
	float nearD = 0.1f;
	float farD = std::max(
		static_cast<int>(std::sqrt(std::pow(CHUNK_SZ_X * s.g.renderDist, 2) + std::pow(CHUNK_SZ_Z * s.g.renderDist, 2))),
		200);
	glm::mat4	projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

	winU->cam->frustumCullingInit(angle, ratio, nearD, farD);

	chunkManager.init(winU->cam->pos, projection);

	skybox.getShader().use();
	skybox.getShader().setMat4("projection", projection);

	pthread_t threadUpdate;
	int rc = pthread_create(&threadUpdate, NULL, threadUpdateFunction, reinterpret_cast<void*>(threadUpdateArgs));
	if (rc) {
		std::cout << "Error: unable to create thread," << rc << std::endl;
		delete threadUpdateArgs;
		return;
	}

	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
	checkError();
	while (!threadUpdateArgs->quit) {
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
		chunkManager.draw(view, winU->cam);

		// draw skybox
		skybox.draw();

		// draw text
		drawText(window, textRender, lastFps, chunkManager);

		glfwSwapBuffers(window);
		glfwPollEvents();
		checkError();

		// delete chunks if needed
		if (chunkManager.toDelete.size() > 0)
			threadUpdateArgs->deleteLocker.ask = true;  // ask update thread to lock

		if (threadUpdateArgs->deleteLocker.isLocked) {  // when the other thread is locked
			// delete all old chunks
			for (auto it = chunkManager.toDelete.begin(); it != chunkManager.toDelete.end(); it++) {
				delete chunkManager.getChunkMap()[*it];
				chunkManager.getChunkMap().erase(*it);
			}
			chunkManager.toDelete.clear();
			threadUpdateArgs->deleteLocker.ask = false;
		}

		// fps
		std::chrono::milliseconds time_loop = getMs() - time_start;
		if (time_loop.count() > loopTime) {
			lastFps = static_cast<int>(1000.0f / time_loop.count());
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					std::cerr << "loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)\n";
			#endif
		}
		else {
			lastFps = s.g.screen.fps;
			usleep((loopTime - time_loop.count()) * 1000);
		}
		firstLoop = false;
	}
	pthread_join(threadUpdate, NULL);  // waiting for the thread to finish
	delete threadUpdateArgs;
}

bool	init(GLFWwindow **window, const char *name, tWinUser *winU, Camera *cam) {
	winU->cam = cam;
	winU->dtTime = 0.0f;
	winU->lastFrame = 0.0f;
	winU->showInfo = true;
	winU->showHelp = true;
	winU->freezeChunkUpdate = false;
	winU->polygonRenderMode = 0;

	if (!initWindow(window, name, winU))
		return (false);
	return (true);
}

int		main(int ac, char const **av) {
	setDefaultSettings();
	try {
		loadSettings(std::string(SETTINGS_FILE));
	}
	catch (Settings::SettingsError &e) {
		return 1;
	}
	if (s.m.seed == 0)
		s.m.seed = time(nullptr);

	if (argparse(ac - 1, av + 1) == false) {
		return 0;
	}

	if (s.m.mapName == "") {  // load without mapName
		std::cout << "[WARN]: no mapname -> you can't save the map" << std::endl;
	}
	else {
		if (createMapFiles() == false) {
			return 1;
		}
		std::cout << "[INFO]: map " << s.m.mapName << std::endl;
	}
	setSeed(s.m.seed);

	GLFWwindow		*window;
	tWinUser		winU;
	Camera			cam(s.m.cameraStartPos.pos, glm::vec3(0, 1, 0), s.m.cameraStartPos.yaw, s.m.cameraStartPos.pitch);
	TextureManager	*textureManager = nullptr;

	std::cout << "[INFO]: starting at " << s.m.cameraStartPos.pos.x << " "
	<< s.m.cameraStartPos.pos.y << " " << s.m.cameraStartPos.pos.z << std::endl;
	std::cout << "[INFO]: random seed " << s.m.seed << std::endl;
	std::cout << "[INFO]: chunk size " << CHUNK_SZ_X << " " << CHUNK_SZ_Y << " " << CHUNK_SZ_Z << std::endl;
	std::cout << "[INFO]: render distance " << s.g.renderDist << " chunks" << std::endl;

	if (!init(&window, "ft_vox", &winU, &cam))
		return (1);
	std::cout << "[INFO]: window size " << s.g.screen.width << " * " << s.g.screen.height << std::endl;

	try {
		// load textures
		textureManager = new TextureManager("./assets/textures.json");

		// load all shaders
		Shader textShader("./shaders/text_vs.glsl", "./shaders/text_fs.glsl");
		Shader skyboxShader("./shaders/skybox_vs.glsl", "./shaders/skybox_fs.glsl");

		// load all fonts
		TextRender textRender(textShader, s.g.screen.width, s.g.screen.height);
		textRender.loadFont("title", s.g.screen.text["title"].path, s.g.screen.text["title"].size);
		textRender.loadFont("normal", s.g.screen.text["normal"].path, s.g.screen.text["normal"].size);

		// load skybox
		Skybox skybox(skyboxShader);

		// create chunkManager
		ChunkManager chunkManager(*textureManager, &winU);

		// run the game
		gameLoop(window, cam, skybox, textRender, chunkManager);

		// save and quit all chunks
		chunkManager.saveAndQuit();
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
	catch (const TextRender::TextRenderError & e) {
		std::cerr << "TextRenderError: " << e.what() << std::endl;
	}

	glfwDestroyWindow(window);
	glfwPollEvents();
	glfwTerminate();

	if (s.m.mapName != "") {  // if we have a map
		std::cout << "[INFO]: saving..." << std::endl;
		if (saveMap(cam))
			std::cout << "[INFO]: settings saved" << std::endl;
		else
			std::cout << "[WARN]: unable to save settings" << std::endl;
	}

	delete textureManager;

	return 0;
}
