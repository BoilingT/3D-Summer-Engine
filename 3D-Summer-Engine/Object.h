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
private:
	int verticesSize;
	unsigned int VBO, VAO, EBO;
public:

	Transform transform;

	void SetupMesh(float* vertices, int size);
	void Draw(Shader& shader);
};

