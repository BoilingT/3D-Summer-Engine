#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"

struct Transform
{
	glm::vec3 pos;
	glm::vec3 dim;
	glm::vec3 rot;
};

class Object
{
private:
	int verticesSize;
	int indicesSize;
	unsigned int *indicesPtr;
	unsigned int VBO, instanceVBO, VAO, EBO;
public:

	Transform transform;
	GLuint *texture;

	void SetupMesh( float *vertices, int vSize );
	void SetupMesh( float *vertices, int vSize, unsigned int *indices, int iSize );
	void Draw( Shader &shader );
	void DrawInstanced( Shader &shader, glm::vec2 *values, int count );

	~Object()
	{
		glDeleteVertexArrays( 1, &VAO );
		glDeleteBuffers( 1, &VBO );
		glDeleteBuffers( 1, &instanceVBO );
		glDeleteBuffers( 1, &EBO );
		VAO = NULL;
		VBO = NULL;
		EBO = NULL;
	}
};

