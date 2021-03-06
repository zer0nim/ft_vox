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
#include "utils/ImageRender.hpp"
#include "utils/CameraCreative.hpp"
#include "utils/CameraSurvival.hpp"

void	*threadUpdateFunction(void *args_) {
	float						loopTime = 1000 / s.g.perf.fps;
	ThreadupdateArgs			*args = reinterpret_cast<ThreadupdateArgs*>(args_);
	std::chrono::milliseconds	time_start;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(args->window));
	uint64_t					nbUpdateCall = 0;
	#if DEBUG_FPS_LOW == true
		bool						firstLoop = true;
	#endif

	while (!args->quit) {
		time_start = getMs();

		args->deleteLocker.isLocked = args->deleteLocker.ask;

		if (args->deleteLocker.isLocked == false) {  // thread is unlocked
			// update
			if (winU->freezeChunkUpdate == false) {
				args->chunkManager.update(winU->cam->pos, args->threadID, nbUpdateCall);
				nbUpdateCall++;
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
					logDebug("update loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)");
			#endif
		}
		else {
			usleep((loopTime - time_loop.count()) * 1000);
		}
		#if DEBUG_SHOW_FPS_ALLTHREAD
			std::cout << "FPS: |update" << static_cast<int>(args->threadID) << "| " << getMs().count() << " | "
				<< (getMs() - time_start).count() << std::endl;
		#endif
		#if DEBUG_FPS_LOW == true
			firstLoop = false;
		#endif
	}
	args->quit = true;
	pthread_exit(NULL);
	return nullptr;
}

void	gameLoop(GLFWwindow *window, Skybox &skybox, TextRender &textRender, ChunkManager &chunkManager, \
ImageRender &imageRender, TextureManager const &textureManager) {
	float						loopTime = 1000 / s.g.perf.fps;
	std::chrono::milliseconds	time_start;
	int							lastFps = 0;
	tWinUser					*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	#if DEBUG_FPS_LOW == true
		bool						firstLoop = true;
	#endif
	float						cursorX = s.g.screen.width / 2 - textRender.font["courrier_new"]['+'].size.x / 2;
	float						cursorY = s.g.screen.height / 2 - textRender.font["courrier_new"]['+'].size.y / 2;
	float						nightCycleCount = 0.0f;

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
	float farD = 200 + static_cast<int>(std::sqrt(std::pow(CHUNK_SZ_X * s.g.perf.renderDist, 2)
		+ std::pow(CHUNK_SZ_Z * s.g.perf.renderDist, 2)));
	glm::mat4	projection = glm::perspective(glm::radians(angle), ratio, nearD, farD);

    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
		winU->camCrea->frustumCullingInit(angle, ratio, nearD, farD);
		winU->camSurv->frustumCullingInit(angle, ratio, nearD, farD);
	}

	CHUNK_OBJECT::initShader(projection, textureManager);  // init shader objects
	chunkManager.init(winU->cam->pos, projection);

	skybox.getShader().use();
	skybox.getShader().setMat4("projection", projection);
	skybox.getShader().unuse();

	for (uint8_t i = 0; i < NB_UPDATE_THREADS; i++) {
		int rc = pthread_create(&(threadUpdate[i]), NULL, threadUpdateFunction, \
		reinterpret_cast<void*>(threadUpdateArgs[i]));
		if (rc) {
			logErr("unable to create thread," << rc);
			return;
		}
	}

	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
	checkError();
	winU->lastFrame = glfwGetTime();
	uint8_t loopCount = 0;  // used to skip first 5 frames from hour count
	nightCycleCount = (s.m.nightCycle.time / 24) * s.m.nightCycle.dayDuration;
	while (!glfwWindowShouldClose(window)) {
		time_start = getMs();

		processInput(window);
		// update camera
		winU->cam->run(winU->dtTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view matrix
		CAMERA_MAT4	view;
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

		CAMERA_MAT4	skyView = view;
		skyView[3][0] = 0;  // remove translation for the skybox
		skyView[3][1] = 0;
		skyView[3][2] = 0;
		skybox.getShader().use();
		skybox.getShader().setMat4("view", skyView);
		skybox.getShader().unuse();

		if (s.m.nightCycle.cycleEnabled) {
			if (loopCount > 5) {
				nightCycleCount += winU->dtTime;
				nightCycleCount = nightCycleCount > s.m.nightCycle.dayDuration \
				? 0.0 : nightCycleCount;
			}
			else {
				++loopCount;
			}
			winU->hour = nightCycleCount / s.m.nightCycle.dayDuration * 24;
		}

		float nightProgress = 0.0f;
		// calculate nightProgress according to time
		// night
		if (winU->hour < s.m.nightCycle.sunriseStart || winU->hour > \
		s.m.nightCycle.sunsetEnd) {
			nightProgress = 1.0f;
		}
		// day
		else if (winU->hour > s.m.nightCycle.sunriseEnd && winU->hour < \
		s.m.nightCycle.sunsetStart) {
			nightProgress = 0.0f;
		}
		// sunset
		else if (winU->hour >= s.m.nightCycle.sunsetStart) {
			nightProgress = (winU->hour - s.m.nightCycle.sunsetStart) / \
			(s.m.nightCycle.sunsetEnd - s.m.nightCycle.sunsetStart);
		}
		// sunrise
		else {
			nightProgress = 1 - ((winU->hour - s.m.nightCycle.sunriseStart) \
			/ (s.m.nightCycle.sunriseEnd - s.m.nightCycle.sunriseStart));
		}

		// draw here
		chunkManager.draw(winU->cam, nightProgress, winU->pointLight);

		// draw skybox
		skybox.draw(nightProgress);

		// draw text
		drawText(window, textRender, lastFps, chunkManager);

		// drawCursor
		textRender.write("courrier_new", "+", cursorX, cursorY, 1, glm::vec3(127, 127, 127));

		// draw image
		drawInventory(window, imageRender, textureManager);

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
					logDebug("loop slow -> " << time_loop.count() << "ms / " << loopTime << "ms (" << FPS << "fps)");
			#endif
		}
		else {
			lastFps = s.g.perf.fps;
			usleep((loopTime - time_loop.count()) * 1000);
		}
		#if DEBUG_SHOW_FPS
			std::cout << "FPS: |main| " << getMs().count() << " | " << (getMs() - time_start).count() << std::endl;
		#endif
		#if DEBUG_FPS_LOW == true
			firstLoop = false;
		#endif
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
	winU->hour = s.m.nightCycle.time;
	winU->runMode = false;
	winU->showInventory = true;
	winU->showInfo = DEBUG;
	winU->showHelp = false;
	winU->showCommands = false;
	winU->showObjList = false;
	winU->freezeChunkUpdate = false;
	winU->putBlock = false;
	winU->destroyBlock = false;
	winU->pointLight = false;
	winU->polygonRenderMode = 0;

	if (!initWindow(window, name, winU))
		return (false);
	return (true);
}

int		main(int ac, char const **av) {
	GLFWwindow		*window = nullptr;
    {
		// init logging
		#if DEBUG
			logging.setLoglevel(LOGDEBUG);
			logging.setPrintFileLine(LOGWARN, true);
			logging.setPrintFileLine(LOGERROR, true);
			logging.setPrintFileLine(LOGFATAL, true);
		#else
			logging.setLoglevel(LOGINFO);
		#endif
		setDefaultSettings();

		if (argparse(ac - 1, av + 1, true) == false) {
			return 0;
		}

		try {
			loadSettings(s.g.files.settingsFile);
		}
		catch (Settings::SettingsError &e) {
			return 1;
		}
		if (s.m.seed == 0) {
			uint32_t seedRand = time(nullptr);
			s.m.seed = rand_r(&seedRand);
		}

		if (argparse(ac - 1, av + 1, false) == false) {
			return 0;
		}

		if (s.m.mapName == "") {  // load without mapName
			logWarn("no mapname -> you can't save the map");
			if (s.m.cameraStartPos.pos.y < 0) {
				setSeed(s.m.seed);
				s.m.cameraStartPos.pos.x += 0.5;
				s.m.cameraStartPos.pos.z += 0.5;
				s.m.cameraStartPos.pos.y = getDefaultElevation(s.m.cameraStartPos.pos.x, s.m.cameraStartPos.pos.z);
			}
		}
		else {
			if (createMapFiles() == false) {
				return 1;
			}
		}
		setSeed(s.m.seed);

		tWinUser		winU;
		Camera			*camCrea = new CameraCreative(s.m.cameraStartPos.pos, glm::vec3(0, 1, 0),
			s.m.cameraStartPos.yaw, s.m.cameraStartPos.pitch);
		camCrea->movementSpeed = s.g.player.creative.movementSpeed;
		camCrea->mouseSensitivity = s.g.player.mouseSensitivity;
		camCrea->runFactor = s.g.player.creative.runFactor;
		Camera			*camSurv = new CameraSurvival(&winU, s.m.cameraStartPos.pos, glm::vec3(0, 1, 0),
			s.m.cameraStartPos.yaw, s.m.cameraStartPos.pitch);
		camSurv->movementSpeed = s.g.player.survival.movementSpeed;
		camSurv->mouseSensitivity = s.g.player.mouseSensitivity;
		camSurv->runFactor = s.g.player.survival.runFactor;
		dynamic_cast<CameraSurvival *>(camSurv)->gravity = s.g.player.survival.gravity;
		dynamic_cast<CameraSurvival *>(camSurv)->jumpHeight = s.g.player.survival.jumpHeight;
		dynamic_cast<CameraSurvival *>(camSurv)->jumpSpeed = s.g.player.survival.jumpSpeed;
		dynamic_cast<CameraSurvival *>(camSurv)->height = s.g.player.survival.height;
		dynamic_cast<CameraSurvival *>(camSurv)->eyeHeight = s.g.player.survival.eyeHeight;
		dynamic_cast<CameraSurvival *>(camSurv)->radius = s.g.player.survival.radius;
		TextureManager	*textureManager = nullptr;

		logInfo("chunk size " << CHUNK_SZ_X << " " << CHUNK_SZ_Y << " " << CHUNK_SZ_Z
			<< " -> render distance " << s.g.perf.renderDist << " chunks");

		if (!init(&window, "ft_vox (F3 to show debug module)", &winU, camCrea, camSurv))
			return (1);
		logInfo("window size " << s.g.screen.width << " * " << s.g.screen.height);

		try {
			// load textures
			textureManager = new TextureManager("./assets/textures.json");

			// load all fonts
			TextRender textRender(s.g.screen.width, s.g.screen.height);
			textRender.loadFont("title", s.g.screen.text["title"].path, s.g.screen.text["title"].size);
			textRender.loadFont("normal", s.g.screen.text["normal"].path, s.g.screen.text["normal"].size);
			textRender.loadFont("courrier_new", s.g.screen.text["courrier_new"].path, s.g.screen.text["courrier_new"].size);

			// load image render
			ImageRender imageRender(*textureManager, s.g.screen.width, s.g.screen.height);
			imageRender.getShader().use();
			textureManager->setUniform(imageRender.getShader());
			imageRender.getShader().unuse();

			// load skybox
			Skybox skybox;

			// create chunkManager
			ChunkManager chunkManager(*textureManager, &winU);
			winU.chunkManager = &chunkManager;

			// run the game
			gameLoop(window, skybox, textRender, chunkManager, imageRender, *textureManager);

			// save and quit all chunks
			chunkManager.saveAndQuit();
		}
		catch(const TextureManager::TextureManagerError& e) {
			logErr("when loading textures: " << e.what());

			glfwDestroyWindow(window);
			window = nullptr;
			glfwPollEvents();
			glfwTerminate();
			return 1;
		}
		catch(const Shader::ShaderError& e) {
			logErr("when loading shader: " << e.what());
		}
		catch (const TextRender::TextRenderError & e) {
			logErr("when loading TextRender: " << e.what());
		}

		if (s.m.mapName != "") {  // if we have a map
			logInfo("saving...");
			if (saveMap(window, winU.cam)) {
				logInfo("settings saved");
			}
			else {
				logWarn("unable to save settings");
			}
		}

		delete textureManager;
		delete winU.camCrea;
		delete winU.camSurv;
		AChunk::deleteShader();
	}

	glfwDestroyWindow(window);
	window = nullptr;
	glfwPollEvents();
	glfwTerminate();

	return 0;
}
