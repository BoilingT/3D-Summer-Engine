#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h> //Needs to be included before GLFW


class Compute
{
private:
	unsigned int computeShader;
public:
	Compute() {
		computeShader = glCreateShader(GL_COMPUTE_SHADER);
	}
};

