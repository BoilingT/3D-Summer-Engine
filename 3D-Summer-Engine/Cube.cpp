#include "Cube.h"

void Cube::SetupMesh() {

	/*for (unsigned int i = 0; i < (sizeof(cubeVertices) / sizeof(float)) / 3; i++)
	{
		Vertex v;
		v.Pos = glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
		vertices.push_back(v);
	}*/

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);

	//Send indices data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

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

void Cube::Draw(Shader& shader) {
	//Draw mesh
	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0)

	glm::mat4 modelM = glm::mat4(1.0f);
	shader.setMat4f("model", modelM);

	glDrawArrays(GL_TRIANGLES, 0, (sizeof(cubeVertices) / sizeof(float)) / 3);
	glBindVertexArray(0);
}