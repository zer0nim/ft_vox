#include <unistd.h>

#include "ft_vox.hpp"
#include "AChunk.hpp"
#include "Chunk.hpp"
#include "utils/Shader.hpp"
#include "utils/Skybox.hpp"

void	gameLoop(GLFWwindow *window, Camera const &cam, Shader &skyboxSh, Skybox &skybox) {
	tWinUser	*winU;
	std::chrono::milliseconds time_start;
	bool firstLoop = true;

	winU = reinterpret_cast<tWinUser *>(glfwGetWindowUserPointer(window));

	// projection matrix
	glm::mat4	projection = glm::perspective(
		glm::radians(cam.zoom), winU->width / winU->height, 0.1f, 100.0f);

	skyboxSh.use();
	skyboxSh.setMat4("projection", projection);

	glClearColor(0.11373f, 0.17647f, 0.27059f, 1.0f);
	checkError();
	while (!glfwWindowShouldClose(window)) {
		time_start = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
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

		skybox.draw();  // draw shader

		glfwSwapBuffers(window);
		glfwPollEvents();
		checkError();

		// fps
		std::chrono::milliseconds time_loop = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()) - time_start;
		if (time_loop.count() > LOOP_TIME) {
			#if DEBUG == true
				if (!firstLoop)
					std::cerr << "loop slow -> " << time_loop.count() << \
					"ms / " << LOOP_TIME << "ms (" << FPS << "fps)\n";
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

int		main() {
	GLFWwindow	*window;
	tWinUser	winU;
	Camera		cam(glm::vec3(0.0f, 0.0f, 3.0f));
	AChunk		*chunk;

	if (!init(&window, "ft_vox", &winU, &cam))
		return (1);

	try {
		Shader skyboxShader("./shaders/skybox_vs.glsl", "./shaders/skybox_fs.glsl");
		Skybox skybox(skyboxShader);

		chunk = new Chunk;
		chunk->createChunk();
		chunk->updateBlock(chunkVec3(2, 3, 4), 3);

		gameLoop(window, cam, skyboxShader, skybox);

		delete chunk;
	}
	catch(Shader::ShaderError & e) {
		std::cerr << e.what() << '\n';
	}

	glfwDestroyWindow(window);
	glfwPollEvents();
	glfwTerminate();

	return 0;
}
