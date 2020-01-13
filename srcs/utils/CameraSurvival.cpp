#include "CameraSurvival.hpp"
#include "ChunkManager.hpp"

CameraSurvival::CameraSurvival(tWinUser * winU, CAMERA_VEC3 pos, CAMERA_VEC3 up, CAMERA_FLOAT yaw, CAMERA_FLOAT pitch)
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

void CameraSurvival::run(CAMERA_FLOAT dtTime) {
	if (_curJumping >= 0 && _curJumping <= jumpHeight) {
		_curJumping += jumpSpeed * dtTime;
		CAMERA_VEC3 tmpPos = pos;
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

void CameraSurvival::processKeyboard(CamMovement direction, CAMERA_FLOAT dtTime, bool isRun) {
	CAMERA_FLOAT	velocity;

	velocity = movementSpeed * dtTime * ((isRun) ? runFactor : 1);
	if (direction == CamMovement::Forward) {
		CAMERA_VEC3 tmpFront = front;
		tmpFront.y = 0;
		tmpFront = glm::normalize(tmpFront);
		_move(pos + tmpFront * velocity);
	}
	if (direction == CamMovement::Backward) {
		CAMERA_VEC3 tmpFront = front;
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
	CAMERA_VEC3 posBottom = pos + (worldUp * -eyeHeight);
	CAMERA_VEC3 posUp = pos + (worldUp * (height - eyeHeight));

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

void CameraSurvival::_move(CAMERA_VEC3 dest, bool _underMove) {
	Constraints	tmpCons;
	CAMERA_FLOAT		moveLength = glm::length(dest - pos);

	if (_underMove == false && moveLength > MOVING_COLISION_PRECISION) {  // split into multiple move
		// call move multiple times
		CAMERA_VEC3	moveDir = glm::normalize(dest - pos);
		CAMERA_VEC3	startPos = pos;
		CAMERA_VEC3	tmpPos = pos;
		bool		stopMoving = false;

		for (CAMERA_FLOAT i = MOVING_COLISION_PRECISION; i < moveLength; i += MOVING_COLISION_PRECISION) {
			_move(startPos + moveDir * i, true);
			if (tmpPos == pos) {  // if the "move" call doesn't move the player
				stopMoving = true;
				break;
			}
			tmpPos = pos;
		}
		if (stopMoving == false) {  // if the "move" call doesn't move the player
			_move(dest, true);
		}
		return;
	}
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

CameraSurvival::Constraints CameraSurvival::_getConstraints(CAMERA_VEC3 dest) {
	Constraints constraints = Constraints();
	CAMERA_VEC3 posBottom = dest + (worldUp * -eyeHeight);
	CAMERA_VEC3 posUp = dest + (worldUp * (height - eyeHeight));
	CAMERA_VEC3 tmpPos;

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
	for (CAMERA_FLOAT yAdd = 0.1; yAdd <= height; yAdd += 0.5) {
		CAMERA_VEC3 posY = posBottom + (worldUp * yAdd);

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
