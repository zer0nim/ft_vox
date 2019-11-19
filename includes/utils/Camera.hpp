#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "commonInclude.hpp"

#define MOVEMENT_SPEED		10.0f
#define RUN_FACTOR			2.0f
#define MOUSE_SENSITIVITY	0.1f

enum class CamMovement {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class Camera {
	public:
		Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
		Camera(Camera const &src);
		virtual ~Camera();

		Camera &operator=(Camera const &rhs);

		glm::mat4 getViewMatrix() const;
		void processKeyboard(CamMovement direction, float dtTime, bool isRun = false);
		void processMouseMovement(float xOffset, float yOffset, bool \
		constrainPitch = true);
		void processMouseScroll(float yOffset);
		void resetPosition();

		glm::vec3	pos;
		glm::vec3	front;
		glm::vec3	up;
		glm::vec3	right;
		glm::vec3	worldUp;

		float		yaw;
		float		pitch;

		float		movementSpeed;
		float		mouseSensitivity;
		float		zoom;

	private:
		void updateCameraVectors();

		/* used to reset position and rotation */
		glm::vec3	_startPos;
		float		_startYaw;
		float		_startPitch;
};

#endif  // CAMERA_HPP_
