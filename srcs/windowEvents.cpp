#include <chrono>

#include "ft_vox.hpp"

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

	// wasd qe fps camera input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Forward, winU->dtTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Backward, winU->dtTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Left, winU->dtTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
	glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Right, winU->dtTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Up, winU->dtTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        winU->cam->processKeyboard(CamMovement::Down, winU->dtTime);
}

/*
	called on key input
*/
void	keyCb(GLFWwindow *window, int key, int scancode, int action, int mods) {
	(void)scancode;
	(void)mods;
	tWinUser	*winU;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));

	// close program key
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// toggle cursor visibility key
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		toggleCursor(window);

	// reset key
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		winU->cam->resetPosition();
	}

	// F3 key
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
		winU->showInfo = !winU->showInfo;
	}
}

void	mouseCb(GLFWwindow *window, double xPos, double yPos) {
	tWinUser		*winU;
	static float	lastX = SCREEN_W / 2.0;
	static float	lastY = SCREEN_H / 2.0;
	float			xOffset;
	float			yOffset;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));

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
	tWinUser	*winU;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));
	winU->width = width;
	winU->height = height;
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

	*window = glfwCreateWindow(SCREEN_W, SCREEN_H, name, NULL, NULL);
	if (!(*window)) {
		fprintf(stderr, "Fail to create glfw3 window\n");
		glfwTerminate();
		return (false);
	}
	glfwMakeContextCurrent(*window);

	glfwSetFramebufferSizeCallback(*window, frambuffResizeCb);
	glfwSetCursorPosCallback(*window, mouseCb);
	glfwSetKeyCallback(*window, keyCb);

	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);  // anti aliasing
	// glEnable(GL_CULL_FACE);  // face culling
	glEnable(GL_BLEND);  // enable blending (used in textRender)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetWindowUserPointer(*window, winU);
	return (true);
}
