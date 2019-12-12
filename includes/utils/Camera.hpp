#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "commonInclude.hpp"

#define CONSTRAINT_Y		true  // if true, move forward didn't affect Y position
#define MOVEMENT_SPEED		15.0f
#define RUN_FACTOR			3.0f
#define MOUSE_SENSITIVITY	0.1f

enum class CamMovement {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

/* frustum culling parameter */
#define FRCL_INSIDE		0b000000  // inside the camera
#define FRCL_LEFT		0b000001  // outside left to the camera
#define FRCL_RIGHT		0b000010  // outside right to the camera
#define FRCL_TOP		0b000100  // outside top to the camera
#define FRCL_BOTTOM		0b001000  // outside bottom to the camera
#define FRCL_NEAR		0b010000  // outside near to the camera
#define FRCL_FAR		0b100000  // outside far to the camera
#define FRCL_OUTSIDE	0b111111  // outside form unknow side

#define FRCL_IS_INSIDE(frustumRes) (frustumRes == FRCL_INSIDE)
#define FRCL_IS_OUTSIDE(frustumRes) (frustumRes > FRCL_INSIDE)

class Camera {
	public:
		Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
		Camera(Camera const &src);
		virtual ~Camera();

		Camera &operator=(Camera const &rhs);

		virtual void run(float dtTime);  // call this function each frame
		glm::mat4 getViewMatrix() const;
		virtual void processKeyboard(CamMovement direction, float dtTime, bool isRun = false);
		virtual void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
		virtual void processMouseScroll(float yOffset);
		virtual void resetPosition();

		/*
		check if objects are inside or outside of the camera
		*/
		void	frustumCullingInit(float angleDeg, float ratio, float nearD, float farD);
		int		frustumCullingCheckPoint(glm::vec3 const &point);  // check if a point is inside the camera
		int		frustumCullingCheckCube(glm::vec3 const &startPoint, glm::vec3 &size);  // check for a cube

		glm::vec3	pos;
		glm::vec3	front;
		glm::vec3	up;
		glm::vec3	right;
		glm::vec3	worldUp;

		float		yaw;
		float		pitch;

		float		movementSpeed;
		float		mouseSensitivity;
		float		runFactor;
		float		zoom;

	private:
		void updateCameraVectors();

		/* used to reset position and rotation */
		glm::vec3	_startPos;
		float		_startYaw;
		float		_startPitch;

		struct FrustumCulling {  // frustum culling parameters
			bool	enabled;
			float	nearD;
			float	farD;
			float	width;
			float	height;
			float	tang;
			float	ratio;

			FrustumCulling() : enabled(false) {}
		};
		FrustumCulling	_frustumCulling;
};

#endif  // CAMERA_HPP_
