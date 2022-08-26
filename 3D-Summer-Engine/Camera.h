#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	//Transform
	//World Space Coordinate System
	const glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f);
	//Directions relative to the cameras coordinate system

public:
	glm::vec3 cameraPos;
	glm::vec3 cameraRot;
	glm::vec3 cameraDir;
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;

	Camera(const glm::vec3 position, const glm::vec3 rotation);

	//Setters
	void setPos(const glm::vec3 pos);
	void setRot(const glm::vec3 rot);
	void setTarget(const glm::vec3 target);

	void updateRelativeCoordinates();
	void translate(const glm::vec3 translation);
	void rotate(const glm::vec3 translation);
};

