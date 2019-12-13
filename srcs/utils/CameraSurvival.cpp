#include "CameraSurvival.hpp"
#include "ChunkManager.hpp"

CameraSurvival::CameraSurvival(tWinUser * winU, glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
: Camera(pos, up, yaw, pitch),
  gravity(SURV_GRAVITY),
  jumpHeight(SURV_JUMP_HEIGHT),
  jumpSpeed(SURV_JUMP_SPEED),
  height(SURV_HEIGHT),
  radius(SURV_RADIUS),
  _winU(winU),
  _curJumping(-1) {
	  movementSpeed = SURV_MOVEMENT_SPEED;
	  runFactor = SURV_RUN_FACTOR;
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
	if (_curJumping >= 0 && _curJumping <= jumpHeight) {
		_curJumping += jumpSpeed * dtTime;
		glm::vec3 tmpPos = pos;
		// move up
		_move(pos + worldUp * jumpSpeed * dtTime);
		if (tmpPos.y == pos.y) {
			_curJumping = -1;
		}
	}
	else if (_curJumping >= 0 && _curJumping <= jumpHeight * 2) {
		_curJumping += jumpSpeed * dtTime;
		// slow gravity
		_move(pos - worldUp * jumpSpeed * dtTime);
	}
	else {
		_curJumping = -1;
		// gravity
		_move(pos - worldUp * gravity * dtTime);
	}
}

void CameraSurvival::processKeyboard(CamMovement direction, float dtTime, bool isRun) {
	float	velocity;

	velocity = movementSpeed * dtTime * ((isRun) ? runFactor : 1);
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
		Constraints tmpCons = _getConstraints(pos + -worldUp * gravity * dtTime);

		if (_curJumping < 0 && tmpCons.negativeY == true) {  // if we are on the ground
			_curJumping = 0;
		}
	}
	if (direction == CamMovement::Down) {
		// unsused direction
	}
}

void CameraSurvival::resetPosition() {}

bool CameraSurvival::isOnBlock(wordIVec3 blockPos) const {
	glm::vec3 posBottom = pos + (worldUp * -eyeHeight);
	glm::vec3 posUp = pos + (worldUp * (height - eyeHeight));

	// check out of X
	if (blockPos.x + 1 < posBottom.x - radius || blockPos.x > posBottom.x + radius)
		return false;

	// check out of Y
	if (blockPos.y + 1 < posBottom.y || blockPos.y > posUp.y)
		return false;

	// check out of Z
	if (blockPos.z + 1 < posBottom.z - radius || blockPos.z > posBottom.z + radius)
		return false;

	return true;
}

void CameraSurvival::_move(glm::vec3 dest) {
	Constraints tmpCons;
    { std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
		tmpCons = _getConstraints(dest);
	}

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

CameraSurvival::Constraints CameraSurvival::_getConstraints(glm::vec3 dest) {
	Constraints constraints = Constraints();
	glm::vec3 posBottom = dest + (worldUp * -eyeHeight);
	glm::vec3 posUp = dest + (worldUp * (height - eyeHeight));
	glm::vec3 tmpPos;

	// Y constraints
	tmpPos = posBottom; tmpPos.x += radius; tmpPos.z += radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeY = true;

	tmpPos = posBottom; tmpPos.x -= radius; tmpPos.z += radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeY = true;

	tmpPos = posBottom; tmpPos.x += radius; tmpPos.z -= radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeY = true;

	tmpPos = posBottom; tmpPos.x -= radius; tmpPos.z -= radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeY = true;

	tmpPos = posUp; tmpPos.x += radius; tmpPos.z += radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveY = true;

	tmpPos = posUp; tmpPos.x -= radius; tmpPos.z += radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveY = true;

	tmpPos = posUp; tmpPos.x += radius; tmpPos.z -= radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveY = true;

	tmpPos = posUp; tmpPos.x -= radius; tmpPos.z -= radius;
	if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveY = true;

	// test at a few height
	for (float yAdd = 0.1; yAdd <= height; yAdd += 0.5) {
		glm::vec3 posY = posBottom + (worldUp * yAdd);

		// X constraints
		tmpPos = posY; tmpPos.x -= radius;
		if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeX = true;

		tmpPos = posY; tmpPos.x += radius;
		if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveX = true;

		// Z constraints
		tmpPos = posY; tmpPos.z -= radius;
		if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.negativeZ = true;

		tmpPos = posY; tmpPos.z += radius;
		if (_winU->chunkManager->getBlock(tmpPos) > 0) constraints.positiveZ = true;
	}

	return constraints;
}
