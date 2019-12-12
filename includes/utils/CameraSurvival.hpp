#pragma once

#include "Camera.hpp"
#include "ft_vox.hpp"

#define SURV_GRAVITY		10.0f
#define SURV_JUMP_HEIGHT	1.2f
#define SURV_HEIGHT			1.8f
#define SURV_RADIUS			0.4f
#define SURV_JUMP_SPEED		10.0f
#define SURV_MOVEMENT_SPEED 12.0f
#define SURV_RUN_FACTOR		1.5f

class CameraSurvival : public Camera {
	public:
		CameraSurvival(tWinUser * winU, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float gravity = SURV_GRAVITY,
		float jumpHeight = SURV_JUMP_HEIGHT, float height = SURV_HEIGHT, float radius = SURV_RADIUS);
		CameraSurvival(CameraSurvival const &src);
		virtual ~CameraSurvival();

		CameraSurvival &operator=(CameraSurvival const &rhs);

		virtual void run(float dtTime);  // call  this function each frame
		virtual void processKeyboard(CamMovement direction, float dtTime, bool isRun = false);
		virtual void resetPosition();  // this function does nothing

	private:
		struct Constraints {  // true == unable to move
			bool	positiveX;
			bool	positiveY;
			bool	positiveZ;
			bool	negativeX;
			bool	negativeY;
			bool	negativeZ;

			Constraints() : positiveX(false), positiveY(false), positiveZ(false),
				negativeX(false), negativeY(false), negativeZ(false) {}
		};

		CameraSurvival();
		void _move(glm::vec3 dest);
		Constraints _getConstraints();

		tWinUser *	_winU;
		float		_height;
		float		_radius;
		float		_gravity;
		float		_jumpHeight;
		float		_jumpSpeed;

		float		_curJumping;  // current jumping height -> -1 == no jump
};
