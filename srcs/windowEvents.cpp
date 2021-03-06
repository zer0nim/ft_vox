#include <chrono>

#include "ft_vox.hpp"
#include "ChunkManager.hpp"
#include "TextureManager.hpp"

static u_int8_t	firstTwoCall = 2;

/*
	toggle cursor visibility
*/
void toggleCursor(GLFWwindow *window) {
	static bool enable = false;

	enable = !enable;
	if (enable)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/*
	called every frame
*/
void	processInput(GLFWwindow *window) {
	tWinUser	*winU;
	float		crntFrame;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	crntFrame = glfwGetTime();
	winU->dtTime = crntFrame - winU->lastFrame;
	winU->lastFrame = crntFrame;

	// wasd qe shift fps camera input
	bool isRun = false;
	if (winU->runMode || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		isRun = true;
	if (winU->runMode || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Forward, winU->dtTime, isRun);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Backward, winU->dtTime, isRun);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Left, winU->dtTime, isRun);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Right, winU->dtTime, isRun);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Up, winU->dtTime, isRun);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Down, winU->dtTime, isRun);
}

/*
	called on key input
*/
bool launchF3Cmd = false;
void	keyCb(GLFWwindow *window, int key, int scancode, int action, int mods) {
	(void)scancode;
	(void)mods;
	tWinUser	*winU;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));

	// close program key
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// toggle cursor visibility key
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		toggleCursor(window);

	// enable / disable fog
	if (key == GLFW_KEY_F && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
		s.g.fog.enabled = !s.g.fog.enabled;
		AChunk::getShader().use();
		AChunk::getShader().setBool("fog.enabled", s.g.fog.enabled);
		AChunk::getShader().unuse();
	}

	// prev item
	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
		s.m.handBlockID--;
		if (s.m.handBlockID <= 0) {
			s.m.handBlockID = NB_TYPE_BLOCKS;
		}
	}
	// next item
	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
		s.m.handBlockID++;
		if (s.m.handBlockID > NB_TYPE_BLOCKS) {
			s.m.handBlockID = 1;
		}
	}

	// gamemode key
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
	    { std::lock_guard<std::mutex>	guard(s.mutexCamera);
			Camera * tmp = winU->cam;
			if (s.m.gamemode == GAMEMODE_CREATIVE) {
				s.m.gamemode = GAMEMODE_SURVIVAL;
				winU->cam = winU->camSurv;
			}
			else {
				s.m.gamemode = GAMEMODE_CREATIVE;
				winU->cam = winU->camCrea;
			}
			winU->cam->pos = tmp->pos;
			winU->cam->front = tmp->front;
			winU->cam->up = tmp->up;
			winU->cam->yaw = tmp->yaw;
			winU->cam->pitch = tmp->pitch;
			winU->cam->processMouseMovement(0, 0);  // simulate a movement to update camera
		}
	}

	// inventory keep element
	for (int i = GLFW_KEY_1; i <= GLFW_KEY_9; i++) {
		if (key == i && action == GLFW_PRESS) {
			int nb = i - GLFW_KEY_1 + 1;
			if (nb >= 1 && nb <= NB_TYPE_BLOCKS) {
				s.m.handBlockID = nb;
			}
		}
	}

	// enable / disable transparency
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		s.g.perf.enableTransparency = !s.g.perf.enableTransparency;
		AChunk::getShader().use();
		AChunk::getShader().setBool("enableTransparency", s.g.perf.enableTransparency);
		AChunk::getShader().unuse();
	}

	// enable / disable point light
	if (key == GLFW_KEY_L && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
		winU->pointLight = !winU->pointLight;
	}

	// F3 key -> debug mode
	if (key == GLFW_KEY_F3 && action == GLFW_RELEASE) {
		if (launchF3Cmd == false)
			winU->showInfo = !winU->showInfo;
		launchF3Cmd = false;
	}

	// F3 + W -> toogle runMode
	if (key == GLFW_KEY_W && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		winU->runMode = !winU->runMode;
		launchF3Cmd = true;
	}

	// F3 + I -> toogle inventory
	if (key == GLFW_KEY_I && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		winU->showInventory = !winU->showInventory;
		launchF3Cmd = true;
	}

	// F3 + F -> freeze chunk update
	if (key == GLFW_KEY_F && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		winU->freezeChunkUpdate = !winU->freezeChunkUpdate;
		launchF3Cmd = true;
	}

	// F3 + R -> reset key
	if (key == GLFW_KEY_R && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		winU->cam->resetPosition();
	}

	// F3 + H -> toggle help menu
	if (key == GLFW_KEY_H && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		if (winU->showInfo == false) {
			winU->showHelp = true;
			winU->showInfo = true;
		}
		else {
			if (winU->showCommands == true && winU->showHelp == true)
				winU->showCommands = false;
			else
				winU->showHelp = !winU->showHelp;
		}
		launchF3Cmd = true;
	}

	// F3 + C -> toggle commands list
	if (key == GLFW_KEY_C && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		if (winU->showInfo == false) {
			winU->showCommands = true;
			winU->showInfo = true;
		}
		else {
			winU->showCommands = !winU->showCommands;
		}
		launchF3Cmd = true;
	}

	// F3 + L -> toggle block list
	if (key == GLFW_KEY_L && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		if (winU->showInfo == false) {
			winU->showObjList = true;
			winU->showInfo = true;
		}
		else {
			winU->showObjList = !winU->showObjList;
		}
		launchF3Cmd = true;
	}

	// F3 + P -> toggle polygon render mode (F3 + P)
	if (key == GLFW_KEY_P && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		winU->polygonRenderMode = (winU->polygonRenderMode + 1) % 3;
		if (winU->polygonRenderMode == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (winU->polygonRenderMode == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
	}
}

void	mouseClick(GLFWwindow *window, int button, int action, int mods) {
	tWinUser	*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	(void)action;
	(void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		winU->destroyBlock = true;
	}
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		winU->destroyBlock = false;
	}
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		winU->putBlock = true;
	}
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		winU->putBlock = false;
	}
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
	    { std::lock_guard<std::mutex>	guard(s.mutexOthers);
			if (winU->chunkManager->raycast.isBlockSelected) {
				s.m.handBlockID = winU->chunkManager->raycast.blockType;
			}
		}
	}
}

void scrolling(GLFWwindow* window, double xoffset, double yoffset) {
	(void)window;

	double offset = yoffset;
	if (offset == 0) {
		offset = xoffset;
	}
	if (s.g.player.inverseScrolling) {
		offset = -offset;
	}
	if (offset > 0) {
		s.m.handBlockID++;
		if (s.m.handBlockID > NB_TYPE_BLOCKS) {
			s.m.handBlockID = 1;
		}
	}
	else if (offset < 0) {
		s.m.handBlockID--;
		if (s.m.handBlockID <= 0) {
			s.m.handBlockID = NB_TYPE_BLOCKS;
		}
	}
}

void	mouseCb(GLFWwindow *window, double xPos, double yPos) {
	tWinUser		*winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	static float	lastX = s.g.screen.width / 2.0;
	static float	lastY = s.g.screen.height / 2.0;
	float			xOffset;
	float			yOffset;

	if (firstTwoCall) {
		lastX = xPos;
		lastY = yPos;
		--firstTwoCall;
	}

	xOffset = xPos - lastX;
	// reversed since y-coordinates go from bottom to top
	yOffset = lastY - yPos;
	winU->cam->processMouseMovement(xOffset, yOffset);

	lastX = xPos;
	lastY = yPos;
}

/*
	windows resize cb
*/
void	frambuffResizeCb(GLFWwindow *window, int width, int height) {
	(void)window;
	s.g.screen.width = width;
	s.g.screen.height = height;
	glViewport(0, 0, width, height);
}

/*
	init windows and context
*/
bool	initWindow(GLFWwindow **window, const char *name, tWinUser *winU) {
	if (!glfwInit()) {
		fprintf(stderr, "Could not start glfw3\n");
		return (false);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);  // anti aliasing

	GLFWmonitor *		monitor = nullptr;
	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *	mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	if (s.g.screen.fullscreen) {
		s.g.screen.width = mode->width;
		s.g.screen.height = mode->height;
	}
	else {
		monitor = nullptr;
	}

	// *window = glfwCreateWindow(s.g.screen.width, s.g.screen.height, name, monitor, NULL);
	*window = glfwCreateWindow(s.g.screen.width, s.g.screen.height, name, monitor, NULL);
	if (!(*window)) {
		fprintf(stderr, "Fail to create glfw3 window\n");
		glfwTerminate();
		return (false);
	}
	glfwMakeContextCurrent(*window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(*window, frambuffResizeCb);
	glfwSetCursorPosCallback(*window, mouseCb);
	glfwSetMouseButtonCallback(*window, mouseClick);
	glfwSetScrollCallback(*window, scrolling);
	glfwSetKeyCallback(*window, keyCb);

	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);  // anti aliasing
	glEnable(GL_CULL_FACE);  // face culling
	glEnable(GL_BLEND);  // enable blending (used in textRender)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetWindowUserPointer(*window, winU);
	return (true);
}
