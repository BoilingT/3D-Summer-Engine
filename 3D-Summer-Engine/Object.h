#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"

struct Transform {
	glm::vec3 pos;
	glm::vec3 dim;
	glm::vec3 rot;
};

class Object
{
public:
	unsigned int VBO, VAO, EBO;
	int verticesSize;

	Transform transform;

	void SetupMesh(float* vertices, int size);
	void Draw(Shader& shader);
};

