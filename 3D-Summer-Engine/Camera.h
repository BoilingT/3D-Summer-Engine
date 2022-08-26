#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	//Transform
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraDir;
	glm::vec3 cameraRot;
	//Coordinate System
	const glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDir;
	glm::vec3 cameraForward;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;

public:
	Camera(const glm::vec3 position, const glm::vec3 rotation);

	//Setters
	void setPos(const glm::vec3 pos);
	void setRot(const glm::vec3 rot);
	void setTarget(const glm::vec3 target);

	//Getters
	glm::vec3 getPos();
	glm::vec3 getRot();
	glm::vec3 getTarget();

	glm::vec3 forward();
	glm::vec3 up();
	glm::vec3 right();

	void translate(const glm::vec3 translation);
	void rotate(const glm::vec3 translation);
};

