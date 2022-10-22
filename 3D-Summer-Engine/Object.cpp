#include "Object.h"

void Object::SetupMesh(float* vertices, int vSize, unsigned int* indices, int iSize) {

	verticesSize = vSize;
	indicesSize = iSize;
	indicesPtr = indices;

	//VAOs
	glGenVertexArrays(1, &VAO);
	//VBOs
	glGenBuffers(1, &VBO);
	//EBOs
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//Send vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vSize, vertices, GL_DYNAMIC_DRAW);
	//Send indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indices, GL_DYNAMIC_DRAW);

	//Vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	/*
	//Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	//Vertex texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	*/
	glBindVertexArray(0);
}

void Object::SetupMesh(float* vertices, int vSize) {

	//VAOs
	glGenVertexArrays(1, &VAO);
	//VBOs
	glGenBuffers(1, &VBO);
	//EBOs
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//Send vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vSize, vertices, GL_DYNAMIC_DRAW);
	verticesSize = vSize;

	//Vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	/*
	//Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	//Vertex texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	*/
	glBindVertexArray(0);
}

void Object::Draw(Shader& shader) {
	//Draw mesh
	glBindVertexArray(VAO);

	glm::mat4 modelM = glm::mat4(1.0f);

	modelM = glm::translate(modelM, transform.pos);
	modelM = glm::scale(modelM, transform.dim);
	//modelM = glm::rotate(modelM, transform.rot.y, glm::vec3(0.0f, 1.0f, 1.0f));

	shader.setMat4f("model", modelM);
	if (indicesSize > 0)
	{
		glDrawElements(GL_TRIANGLES, indicesSize / sizeof(float), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (verticesSize / sizeof(float)));
	}
	glBindVertexArray(0);
}