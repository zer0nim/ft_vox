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
		if (time_loop.count() > LOOP_TIME) {
			#if DEBUG_FPS_LOW == true
				if (!firstLoop)
					std::cerr << "update loop slow -> " << time_loop.count() << "ms / " << LOOP_TIME << "ms (" << FPS << "fps)\n";
			#endif
		}
		else {
			usleep((LOOP_TIME - time_loop.count()) * 1000);
		}
		firstLoop = false;
	}
	args->quit = true;
	pthread_exit(NULL);
	return nullptr;
}

void	gameLoop(GLFWwindow *window, Camera const &cam, Skybox &skybox, \
TextRender &textRender, ChunkManager &chunkManager) {
	std::chrono::milliseconds	time_start;
	int							lastFps = 0;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	bool						firstLoop = true;
	ThreadupdateArgs			*threadUpdateArgs = new ThreadupdateArgs(window, chunkManager, winU->cam->pos);

	// projection matrix
	glm::mat4	projection = glm::perspective(
		glm::radians(cam.zoom), winU->width / winU->height, 0.1f, 100.0f);

	chunkManager.init(winU->cam->pos, projection);

	skybox.getShader().use();
	skybox.getShader().setMat4("projection", projection);

	pthread_t threadUpdate;
	int rc = pthread_create(&threadUpdate, NULL, threadUpdateFunction, reinterpret_cast<void*>(threadUpdateArgs));
	if (rc) {
         std::cout << "Error: unable to create thread," << rc << std::endl;
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
		chunkManager.draw(view);

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
		if (time_loop.count() > LOOP_TIME) {
			lastFps = static_cast<int>(1000.0f / time_loop.count());
			#if DEBUG_FPS_LOW == true
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
	winU->showHelp = true;
	winU->freezeChunkUpdate = false;
	winU->polygonRenderMode = 0;

	if (!initWindow(window, name, winU))
		return (false);
	return (true);
}

int		main(int ac, char const **av) {
	GLFWwindow		*window;
	tWinUser		winU;
	glm::vec3		startingPos(0.0f, 64.0f, 19.0f);
	Camera			cam(startingPos);
	TextureManager	*textureManager = nullptr;

	uint32_t randSeed = time(nullptr);
	std::string mapName = "";
	uint32_t	seed = rand_r(&randSeed);
	if (argparse(ac - 1, av + 1, mapName, &seed) == false) {
		return 0;
	}

	if (mapName == "") {  // load without mapName
		std::cout << "[WARN]: no mapname -> you can't save the map" << std::endl;
	}
	else {
		if (createMapFiles(mapName, &seed) == false) {
			return 1;
		}
		std::cout << "[INFO]: map " << mapName << std::endl;
		mapName = std::string(MAPS_PATH) + mapName;
	}
	setSeed(seed);

	std::cout << "[INFO]: starting at " << startingPos.x << " " << startingPos.y << " " << startingPos.z << std::endl;
	std::cout << "[INFO]: random seed " << seed << std::endl;
	std::cout << "[INFO]: chunk size " << CHUNK_SZ_X << " " << CHUNK_SZ_Y << " " << CHUNK_SZ_Z << std::endl;
	std::cout << "[INFO]: render distance " << RENDER_DISTANCE_CHUNK << " chunks" << std::endl;


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
		ChunkManager chunkManager(mapName, *textureManager, &winU);

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
	catch (const TextRender::TextRenderError & e) {
		std::cerr << "TextRenderError: " << e.what() << std::endl;
	}

	delete textureManager;
	glfwDestroyWindow(window);
	glfwPollEvents();
	glfwTerminate();

	return 0;
}
