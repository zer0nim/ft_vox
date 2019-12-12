#include "CameraSurvival.hpp"

CameraSurvival::CameraSurvival(glm::vec3 pos, glm::vec3 up, float yaw, float pitch, float gravity)
: Camera(pos, up, yaw, pitch),
  _gravity(gravity) {
}

CameraSurvival::CameraSurvival(CameraSurvival const &src)
: Camera(src) {
	*this = src;
}

CameraSurvival::~CameraSurvival() {
}

CameraSurvival &CameraSurvival::operator=(CameraSurvival const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void CameraSurvival::run(float dtTime) {
	// gravity
	pos = pos - glm::vec3(0, 1, 0) * _gravity * dtTime;
}

void CameraSurvival::resetPosition() {}
