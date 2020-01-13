#pragma once

#include "Camera.hpp"
#include "commonInclude.hpp"

class CameraCreative : public Camera {
	public:
		CameraCreative(CAMERA_VEC3 pos = CAMERA_VEC3(0.0f, 0.0f, 0.0f), CAMERA_VEC3 up = \
		CAMERA_VEC3(0.0f, 1.0f, 0.0f), CAMERA_FLOAT yaw = -90.0f, CAMERA_FLOAT pitch = 0.0f);
		CameraCreative(CameraCreative const &src);
		virtual ~CameraCreative();

		CameraCreative &operator=(CameraCreative const &rhs);

	private:
		CameraCreative();
};
