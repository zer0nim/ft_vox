#include "CameraCreative.hpp"

CameraCreative::CameraCreative(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
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
