#pragma once

#include "Camera.hpp"

class CameraSurvival : public Camera {
	public:
		CameraSurvival(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float gravity = MOVEMENT_SPEED);
		CameraSurvival(CameraSurvival const &src);
		virtual ~CameraSurvival();

		CameraSurvival &operator=(CameraSurvival const &rhs);

		virtual void run(float dtTime);  // call  this function each frame
		virtual void resetPosition();  // this function does nothing
	protected:
	private:
		CameraSurvival();

		float _gravity;
};
