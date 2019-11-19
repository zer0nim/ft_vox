#include "Camera.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
: pos(pos),
  front(glm::vec3(0.0f, 0.0f, -1.0f)),
  worldUp(up),
  yaw(yaw),
  pitch(pitch),
  movementSpeed(MOVEMENT_SPEED),
  mouseSensitivity(MOUSE_SENSITIVITY),
  zoom(45.0f),
  _startPos(pos),
  _startYaw(yaw),
  _startPitch(pitch) {
	updateCameraVectors();
}

Camera::Camera(Camera const &src) {
	*this = src;
}

Camera::~Camera() {
}

Camera &Camera::operator=(Camera const &rhs) {
	if (this != &rhs) {
		pos = rhs.pos;
		front = rhs.front;
		up = rhs.up;
		right = rhs.right;
		worldUp = rhs.worldUp;
		yaw = rhs.yaw;
		pitch = rhs.pitch;
		movementSpeed = rhs.movementSpeed;
		mouseSensitivity = rhs.mouseSensitivity;
		zoom = rhs.zoom;
	}
	return *this;
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(pos, pos + front, up);
}

void Camera::processKeyboard(CamMovement direction, float dtTime, bool isRun) {
	float	velocity;

	velocity = movementSpeed * dtTime * ((isRun) ? RUN_FACTOR : 1);
	if (direction == CamMovement::Forward) {
		#if CONSTRAINT_Y == true
			glm::vec3 tmpFront = front;
			tmpFront.y = 0;
			tmpFront = glm::normalize(tmpFront);
			pos = pos + tmpFront * velocity;
		#else
			pos = pos + front * velocity;
		#endif
	}
	if (direction == CamMovement::Backward) {
		#if CONSTRAINT_Y == true
			glm::vec3 tmpFront = front;
			tmpFront.y = 0;
			tmpFront = glm::normalize(tmpFront);
			pos = pos - tmpFront * velocity;
		#else
			pos = pos - front * velocity;
		#endif
	}
	if (direction == CamMovement::Left) {
		pos = pos - right * velocity;
	}
	if (direction == CamMovement::Right) {
		pos = pos + right * velocity;
	}
	if (direction == CamMovement::Up) {
		pos = pos + glm::vec3(0, 1, 0) * velocity;
	}
	if (direction == CamMovement::Down) {
		pos = pos - glm::vec3(0, 1, 0) * velocity;
	}
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// constrain pitch to avoid screen flip
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yOffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors() {
	glm::vec3 nFront;

	nFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	nFront.y = sin(glm::radians(pitch));
	nFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(nFront);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::resetPosition() {
	pos = _startPos;
	yaw = _startYaw;
	pitch = _startPitch;

	updateCameraVectors();
}
