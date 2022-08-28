#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	//Directions relative to the World coordinate system
	const glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f);
	//Transform
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraRot = glm::vec3(0.0f, 90.0f, 0.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f);
	//Directions relative to the Camera's coordinate system
	glm::vec3 cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraRight;
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	

public:
	Camera();
	Camera(const glm::vec3 position);

	//Setters
	void setPos(const glm::vec3 pos);
	void setRot(const glm::vec3 rot);

	//Getters
	glm::vec3 getPos();
	glm::vec3 getRot();

	glm::vec3 forward();
	glm::vec3 up();
	glm::vec3 right();

	void processMouseMovement(float xTravel, float yTravel);
	void updateRelativeCoordinates();
	void translate(const glm::vec3 translation);
	void rotate(const glm::vec3 rotation);
};

