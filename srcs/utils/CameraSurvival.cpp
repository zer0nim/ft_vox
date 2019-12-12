#include "CameraSurvival.hpp"

CameraSurvival::CameraSurvival(glm::vec3 pos, glm::vec3 up, float yaw, float pitch, float gravity, float jumpHeight)
: Camera(pos, up, yaw, pitch),
  _gravity(gravity),
  _jumpHeight(jumpHeight) {}

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
	_move(pos - glm::vec3(0, 1, 0) * _gravity * dtTime);
}

void CameraSurvival::processKeyboard(CamMovement direction, float dtTime, bool isRun) {
	float	velocity;

	velocity = movementSpeed * dtTime * ((isRun) ? RUN_FACTOR : 1);
	if (direction == CamMovement::Forward) {
		glm::vec3 tmpFront = front;
		tmpFront.y = 0;
		tmpFront = glm::normalize(tmpFront);
		_move(pos + tmpFront * velocity);
	}
	if (direction == CamMovement::Backward) {
		glm::vec3 tmpFront = front;
		tmpFront.y = 0;
		tmpFront = glm::normalize(tmpFront);
		_move(pos - tmpFront * velocity);
	}
	if (direction == CamMovement::Left) {
		_move(pos - right * velocity);
	}
	if (direction == CamMovement::Right) {
		_move(pos + right * velocity);
	}
	if (direction == CamMovement::Up) {
		_move(pos + glm::vec3(0, 1, 0) * velocity);
	}
	if (direction == CamMovement::Down) {
		// unsused direction
	}
}

void CameraSurvival::resetPosition() {}

void CameraSurvival::_move(glm::vec3 dest) {
	Constraints tmpCons;
	_setConstraints(tmpCons);

	// X constraints
	if (dest.x - pos.x > 0 && tmpCons.positiveX == false)
		pos.x = dest.x;
	else if (dest.x - pos.x < 0 && tmpCons.negativeX == false)
		pos.x = dest.x;

	// Y constraints
	if (dest.y - pos.y > 0 && tmpCons.positiveY == false)
		pos.y = dest.y;
	else if (dest.y - pos.y < 0 && tmpCons.negativeY == false)
		pos.y = dest.y;

	// Z constraints
	if (dest.z - pos.z > 0 && tmpCons.positiveZ == false)
		pos.z = dest.z;
	else if (dest.z - pos.z < 0 && tmpCons.negativeZ == false)
		pos.z = dest.z;
}

void CameraSurvival::_setConstraints(Constraints &constraints) {
	(void)constraints;
}
