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

	if (cameraRot.x > 89.0f)
		cameraRot.x = 89.0f;
	if (cameraRot.x < -89.0f)
		cameraRot.x = -89.0f;
	if (cameraRot.y > 360.0f)
	{
		//cameraRot.y = cameraRot.y - ((int) cameraRot.y % 360) * 360;
		cameraRot.y = 0.0f;
	}
	if (cameraRot.y < 0.0f)
	{
		cameraRot.y = 360.0f;
		//cameraRot.y = cameraRot.y - ((int)cameraRot.y % 360) * 360;
	}

	//Yaw
	cameraDirection.x = cos(glm::radians(cameraRot.y)) * cos(glm::radians(cameraRot.x));
	cameraDirection.z = -1 * sin(glm::radians(cameraRot.y)) * cos(glm::radians(cameraRot.x));
	//Pitch
	cameraDirection.y = sin(glm::radians(cameraRot.x));
	cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUp));
	cameraForward = glm::normalize(cameraDirection);

	cameraRight = glm::normalize(glm::cross(upDir, cameraDirection));
	cameraUp = glm::cross(cameraDirection, cameraRight);


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

void Camera::processKeyboardInput(Camera_Movement dir, float dt) {

	if (dir == Camera_Movement::FORWARD)
	{
		translate(forward() * speed * dt);
	}
	if (dir == Camera_Movement::BACKWARD)
	{
		translate(forward() * -speed * dt);
	}
	if (dir == Camera_Movement::RIGHT)
	{
		translate(right() * -speed * dt);
	}
	if (dir == Camera_Movement::LEFT)
	{
		translate(right() * speed * dt);
	}
	if (dir == Camera_Movement::UP)
	{
		translate(glm::vec3(0.0f, 1.0f, 0.0f) * speed * dt);
	}
	if (dir == Camera_Movement::DOWN)
	{
		translate(glm::vec3(0.0f, 1.0f, 0.0f) * -speed * dt);
	}
}

void Camera::processMouseScroll(float fovOffset) {
	fov -= fovOffset;
	if (fov < 1.0f) {
		fov = 1.0f;
	}
	if (fov > 45.0f) {
		fov = 45.0f;
	}
}

void Camera::processMouseMovement(float xTravel, float yTravel) {
	rotate(glm::vec3(xTravel, yTravel, 0.0f));
}

void Camera::translate(const glm::vec3 translation) {
	cameraPos = cameraPos + translation;
	updateRelativeCoordinates();
	std::cout << "Position: { " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << " }" << std::endl;
}

void Camera::rotate(const glm::vec3 rotation) {
	cameraRot = cameraRot + rotation;
	updateRelativeCoordinates();
	std::cout << "Rotation: { " << cameraRot.x << ", " << cameraRot.y << ", " << cameraRot.z << " }" << std::endl;
}