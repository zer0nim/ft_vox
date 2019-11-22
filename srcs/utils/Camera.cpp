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

/*
init the frustum culling (take the same args as projection matrix)
*/
void Camera::frustumCullingInit(float angleDeg, float ratio, float nearD, float farD) {
	_frustumCulling.tang = static_cast<float>(glm::tan(glm::radians(angleDeg * 0.5)));

	_frustumCulling.ratio = ratio;
	_frustumCulling.nearD = nearD;
	_frustumCulling.farD = farD;

	_frustumCulling.height = _frustumCulling.nearD * _frustumCulling.tang;
	_frustumCulling.width = _frustumCulling.height * _frustumCulling.ratio;

	_frustumCulling.enabled = true;
}

/*
check if a point is inside or outside the camera
*/
int		Camera::frustumCullingCheckPoint(glm::vec3 const &point) {
	float	pcz, pcx, pcy, aux;
	int		res = FRCL_INSIDE;

	if (!_frustumCulling.enabled) {
		std::cerr << "init frustumCulling before using it: frustumCullingInit" << std::endl;
		return FRCL_INSIDE;
	}

	// compute vector from camera position to p
	glm::vec3 point2pos = point - pos;

	// compute and test the Z coordinate to get distance from camera
	pcz = glm::dot(point2pos, front);
	if (pcz > _frustumCulling.farD)
		res |= FRCL_FAR;
	else if (pcz < _frustumCulling.nearD)
 		res |= FRCL_NEAR;

 	// compute and test the Y coordinate to get distance from camera
 	pcy = glm::dot(point2pos, up);
 	aux = pcz * _frustumCulling.tang;
 	if (pcy > aux)
		res |= FRCL_TOP;
	else if (pcy < -aux)
 		res |= FRCL_BOTTOM;

 	// compute and test the X coordinate to get distance from camera
 	pcx = glm::dot(point2pos, right);
 	aux = aux * _frustumCulling.ratio;
 	if (pcx > aux)
		res |= FRCL_RIGHT;
	else if (pcx < -aux)
 		res |= FRCL_LEFT;

	return res;
}

/*
check if a cube is inside or outside of the camera
startPoint is the 0|0|0 coordinate of the cube
size if the scale in X Y and Z
*/
int		Camera::frustumCullingCheckCube(glm::vec3 const &startPoint, glm::vec3 &size) {
	int			res;  // point1 & point2 & point3 ...
	int			tmpRes;
	glm::vec3	pos;

	// compute on all positions
	pos = startPoint;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res = tmpRes;

	pos = startPoint;
	pos.x += size.x;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.y += size.y;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.y += size.y;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.y += size.y;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	pos = startPoint;
	pos.x += size.x;
	pos.y += size.y;
	pos.z += size.z;
	tmpRes = frustumCullingCheckPoint(pos);
	if (tmpRes == FRCL_INSIDE) return FRCL_INSIDE;
	res &= tmpRes;

	if (res > 0)
		return FRCL_OUTSIDE;  // outside
	else
		return FRCL_INSIDE;
}
