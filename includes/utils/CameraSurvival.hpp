#pragma once

#include "Camera.hpp"

#define GRAVITY		MOVEMENT_SPEED
#define JUMP_HEIGHT	1.2f

class CameraSurvival : public Camera {
	public:
		CameraSurvival(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float gravity = GRAVITY,
		float jumpHeight = JUMP_HEIGHT);
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
		void _setConstraints(Constraints &constraints);

		float _gravity;
		float _jumpHeight;
};
