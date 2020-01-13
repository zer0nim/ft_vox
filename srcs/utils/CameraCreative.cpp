#include "CameraCreative.hpp"

CameraCreative::CameraCreative(CAMERA_VEC3 pos, CAMERA_VEC3 up, CAMERA_FLOAT yaw, CAMERA_FLOAT pitch)
: Camera(pos, up, yaw, pitch) {
}

CameraCreative::CameraCreative(CameraCreative const &src)
: Camera(src) {
	*this = src;
}

CameraCreative::~CameraCreative() {
}

CameraCreative &CameraCreative::operator=(CameraCreative const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}
