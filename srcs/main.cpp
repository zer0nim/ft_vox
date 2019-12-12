#include <unistd.h>
#include <pthread.h>
#include <iomanip>

#include "ft_vox.hpp"
#include "TextureManager.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "GreedyChunk.hpp"
#include "GreedyChunk2.hpp"
#include "GreedyChunk3.hpp"
#include "ChunkManager.hpp"
#include "MapGenerator.hpp"
#include "utils/Shader.hpp"
#include "utils/Skybox.hpp"
#include "utils/TextRender.hpp"
#include "utils/Stats.hpp"
#include "utils/CameraCreative.hpp"
#include "utils/CameraSurvival.hpp"

void	*threadUpdateFunction(void *args_) {
	float						loopTime = 1000 / s.g.screen.fps;
	ThreadupdateArgs			*args = reinterpret_cast<ThreadupdateArgs*>(args_);
	std::chrono::milliseconds	time_start;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(args->window));
	bool						firstLoop = true;

	while (!args->quit) {
		time_start = getMs();

		args->deleteLocker.isLocked = args->deleteLocker.ask;

		if (args->deleteLocker.isLocked == false) {  // thread is unlocked
			// update
			if (winU->freezeChunkUpdate == false) {
				args->chunkManager.update(winU->cam->pos, args->threadID);
			    { std::lock_guard<std::mutex>	guard(s.mutexToDelete);
					if (args->chunkManager.toDelete.empty() == false) {  // auto lock
						args->deleteLocker.ask = true;
						continue;
					}
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
		#if DEBUG_SHOW_FPS_ALLTHREAD
			std::cout << "FPS: |update" << static_cast<int>(args->threadID) << "| " << getMs().count() << " | "
				<< (getMs() - time_start).count() << std::endl;
		#endif
		firstLoop = false;
	}
	args->quit = true;
	pthread_exit(NULL);
	return nullptr;
}

void	gameLoop(GLFWwindow *window, Skybox &skybox, \
TextRender &textRender, ChunkManager &chunkManager, TextureManager const &textureManager) {
	float						loopTime = 1000 / s.g.screen.fps;
	std::chrono::milliseconds	time_start;
	int							lastFps = 0;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	bool						firstLoop = true;
	float						cursorX = s.g.screen.width / 2 - textRender.font["courrier_new"]['+'].size.x / 2;
	float						cursorY = s.g.screen.height / 2 - textRender.font["courrier_new"]['+'].size.y / 2;

	/* threading */
	std::array<ThreadupdateArgs *, NB_UPDATE_THREADS>	threadUpdateArgs;
	std::array<pthread_t, NB_UPDATE_THREADS>			threadUpdate;

	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		threadUpdateArgs[i] = new ThreadupdateArgs(i, window, chunkManager);
	}

	// projection matrix
	float angle = winU->cam->zoom;
	float ratio = static_cast<float>(s.g.screen.width) / s.g.screen.height;
	float nearD = 0.1f;
	float farD = 200 + static_cast<int>(std::sqrt(std::pow(CHUNK_SZ_X * s.g.renderDist, 2)
		+ std::pow(CHUNK_SZ_Z * s.g.renderDist, 2)));
	glm::mat4	projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
		winU->camCrea->frustumCullingInit(angle, ratio, nearD, farD);
		winU->camSurv->frustumCullingInit(angle, ratio, nearD, farD);
	}

	CHUNK_OBJECT::initShader(projection, textureManager);  // init shader objects
	chunkManager.init(winU->cam->pos, projection);

	skybox.getShader().use();
	skybox.getShader().setMat4("projection", projection);

	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		int rc = pthread_create(&(threadUpdate[i]), NULL, threadUpdateFunction, reinterpret_cast<void*>(threadUpdateArgs[i]));
		if (rc) {
			std::cout << "Error: unable to create thread," << rc << std::endl;
			return;
		}
	}

	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
	checkError();
	while (!glfwWindowShouldClose(window)) {
		time_start = getMs();

		processInput(window);
		// update camera
		winU->cam->run(winU->dtTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view matrix
		glm::mat4	view;
	    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
			view = winU->cam->getViewMatrix();
		}

		// put / destroy block
		if (winU->putBlock) {
			chunkManager.putBlock(s.m.handBlockID);
		}
		if (winU->destroyBlock) {
			chunkManager.destroyBlock();
		}

		// update raycast
		chunkManager.updateRaycast();

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

		// drawCursor
		textRender.write("courrier_new", "+", cursorX, cursorY, 1, glm::vec3(127, 127, 127));

		glfwSwapBuffers(window);
		glfwPollEvents();
		checkError();

		// delete chunks if needed
	    { std::lock_guard<std::mutex>	guard(s.mutexToDelete);
			if (chunkManager.toDelete.empty() == false) {
				for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
					threadUpdateArgs[i]->deleteLocker.ask = true;  // ask update thread to lock
				}
			}
		}

		bool	allLocked = true;
		for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
			if (threadUpdateArgs[i]->deleteLocker.isLocked == false) {
				allLocked = false;
				break;
			}
		}
		if (allLocked) {  // when the other thread is locked
			// delete all old chunks
		    { std::lock_guard<std::mutex>	guard(s.mutexToDelete);
				while (chunkManager.toDelete.empty() == false) {
					delete chunkManager.getChunkMap()[chunkManager.toDelete.front()];
					chunkManager.getChunkMap().erase(chunkManager.toDelete.front());
					chunkManager.toDelete.pop_front();
				}
			}
			for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
				threadUpdateArgs[i]->deleteLocker.ask = false;
			}
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
		#if DEBUG_SHOW_FPS
			std::cout << "FPS: |main| " << getMs().count() << " | " << (getMs() - time_start).count() << std::endl;
		#endif
		firstLoop = false;
	}

	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		threadUpdateArgs[i]->quit = true;
	}
	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		pthread_join(threadUpdate[i], NULL);
	}
	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		delete threadUpdateArgs[i];
	}

	#if DEBUG_SHOW_FPS
		std::cout << "ENDFPS" << std::endl;
	#endif
}

bool	init(GLFWwindow **window, const char *name, tWinUser *winU, Camera *camCrea, Camera *camSurv) {
	winU->camSurv = camSurv;
	winU->camCrea = camCrea;
	if (s.m.gamemode == GAMEMODE_CREATIVE)
		winU->cam = winU->camCrea;
	else
		winU->cam = winU->camSurv;
	winU->chunkManager = nullptr;
	winU->dtTime = 0.0f;
	winU->lastFrame = 0.0f;
	winU->showInfo = true;
	winU->showHelp = false;
	winU->freezeChunkUpdate = false;
	winU->putBlock = false;
	winU->destroyBlock = false;
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
	if (s.m.seed == 0) {
		uint32_t seedRand = time(nullptr);
		s.m.seed = rand_r(&seedRand);
	}

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
	Camera			*camCrea = new CameraCreative(s.m.cameraStartPos.pos, glm::vec3(0, 1, 0),
		s.m.cameraStartPos.yaw, s.m.cameraStartPos.pitch);
	Camera			*camSurv = new CameraSurvival(&winU, s.m.cameraStartPos.pos, glm::vec3(0, 1, 0),
		s.m.cameraStartPos.yaw, s.m.cameraStartPos.pitch);
	TextureManager	*textureManager = nullptr;

	std::cout << "[INFO]: starting at " << s.m.cameraStartPos.pos.x << " "
	<< s.m.cameraStartPos.pos.y << " " << s.m.cameraStartPos.pos.z << std::endl;
	std::cout << "[INFO]: random seed " << s.m.seed << std::endl;
	std::cout << "[INFO]: chunk size " << CHUNK_SZ_X << " " << CHUNK_SZ_Y << " " << CHUNK_SZ_Z << std::endl;
	std::cout << "[INFO]: render distance " << s.g.renderDist << " chunks" << std::endl;

	if (!init(&window, "ft_vox", &winU, camCrea, camSurv))
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
		textRender.loadFont("courrier_new", s.g.screen.text["courrier_new"].path, s.g.screen.text["courrier_new"].size);

		// load skybox
		Skybox skybox(skyboxShader);

		// create chunkManager
		ChunkManager chunkManager(*textureManager, &winU);
		winU.chunkManager = &chunkManager;

		// run the game
		gameLoop(window, skybox, textRender, chunkManager, *textureManager);
		Stats::printStats();

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
		if (saveMap(winU.cam))
			std::cout << "[INFO]: settings saved" << std::endl;
		else
			std::cout << "[WARN]: unable to save settings" << std::endl;
	}

	delete textureManager;
	delete winU.camCrea;
	delete winU.camSurv;

	return 0;
}
