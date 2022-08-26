#include "Camera.h"

Camera::Camera() {
	updateRelativeCoordinates();
}

Camera::Camera(const glm::vec3 pos) {
	cameraPos = pos;

	updateRelativeCoordinates();
}

void Camera::updateRelativeCoordinates() {
	/*cameraDir = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(upDir, cameraDir));
	cameraUp = glm::cross(cameraDir, cameraRight);*/
	cameraRight = glm::normalize(glm::cross(cameraForward, cameraUp));
}

void Camera::setPos(const glm::vec3 pos) {
	cameraPos = pos;
	updateRelativeCoordinates();
}

void Camera::setRot(const glm::vec3 rot) {
	cameraRot = rot;
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::vec3 Camera::getRot() {
	return cameraRot;
}

glm::vec3 Camera::up() {
	return cameraUp;
}

glm::vec3 Camera::right() {
	return cameraRight;
}

glm::vec3 Camera::forward() {
	return cameraForward;
}

void Camera::translate(const glm::vec3 translation) {
	cameraPos = cameraPos + translation;
	updateRelativeCoordinates();
	std::cout << "Position: { " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << " }" << std::endl;
}

void Camera::rotate(const glm::vec3 rotation) {
	return;
}