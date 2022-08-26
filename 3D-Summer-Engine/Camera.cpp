#include "Camera.h"

Camera::Camera(const glm::vec3 pos, const glm::vec3 rot) {
	cameraPos = pos;
	cameraRot = rot;

	cameraDir = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(upDir, cameraDir));
	cameraUp = glm::cross(cameraDir, cameraRight);
}

void Camera::setPos(const glm::vec3 pos) {
	cameraPos = pos;
}

void Camera::setRot(const glm::vec3 rot) {
	cameraRot = rot;
}

void Camera::setTarget(const glm::vec3 target) {
	cameraTarget = target;
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::vec3 Camera::getRot() {
	return cameraRot;
}

glm::vec3 Camera::getTarget() {
	return cameraTarget;
}

glm::vec3 Camera::up() {
	return cameraUp;
}

glm::vec3 Camera::right() {
	return cameraRight;
}

glm::vec3 Camera::forward() {
	return -cameraDir;
}

void Camera::translate(const glm::vec3 translation) {
	cameraPos = cameraPos + translation;
}
