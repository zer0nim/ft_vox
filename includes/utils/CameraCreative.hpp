#pragma once

#include "Camera.hpp"
#include "commonInclude.hpp"

class CameraCreative : public Camera {
	public:
		CameraCreative(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = \
		glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
		CameraCreative(CameraCreative const &src);
		virtual ~CameraCreative();

		CameraCreative &operator=(CameraCreative const &rhs);
	protected:
	private:
		CameraCreative();
};
