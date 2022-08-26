#include "Camera.h"

Camera::Camera(const glm::vec3 pos, const glm::vec3 rot) {
	cameraPos = pos;
	cameraRot = rot;

	updateRelativeCoordinates();
}

void Camera::updateRelativeCoordinates() {
	cameraDir = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(upDir, cameraDir));
	cameraUp = glm::cross(cameraDir, cameraRight);
}

void Camera::setPos(const glm::vec3 pos) {
	cameraPos = pos;
	updateRelativeCoordinates();
}

void Camera::setRot(const glm::vec3 rot) {
	cameraRot = rot;
}

void Camera::setTarget(const glm::vec3 target) {
	cameraTarget = target;
	updateRelativeCoordinates();
}

void Camera::translate(const glm::vec3 translation) {
	cameraPos = cameraPos + translation;
	cameraTarget = cameraTarget + translation;
	updateRelativeCoordinates();
	std::cout << "Position: { " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << " }" << std::endl;
}