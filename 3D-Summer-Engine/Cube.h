#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

struct Transform {
	glm::vec3 pos;
	glm::vec3 dim;
	glm::vec3 rot;
};

class Cube
{
private:
	unsigned int VBO, VAO, EBO;
public:
	Transform transform;
	Cube(glm::vec3 pos, glm::vec3 dim, glm::vec3 rot) {
		transform.pos = pos;
		transform.dim = dim;
		transform.rot = rot;
	}

	void SetupMesh();
	void Draw(Shader &shader);
};

