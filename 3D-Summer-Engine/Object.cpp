#include "Object.h"

void Object::SetupMesh(float* vertices, int vSize, unsigned int* indices, int iSize) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	verticesSize = vSize;
	indicesSize	 = iSize;
	indicesPtr	 = indices;

	//VAOs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Send vertices data
	glBufferData(GL_ARRAY_BUFFER, vSize, vertices, GL_DYNAMIC_DRAW);
	//Send indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, indices, GL_DYNAMIC_DRAW);

	//Vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);
	//Enable texture coordinates data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
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
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
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

void Object::DrawInstanced(Shader& shader, glm::vec2* values, int count) {
	shader.use();
	glBindTexture(GL_TEXTURE_2D, texture ? *texture : 0);
	glBindVertexArray(VAO);
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec2), values, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);


	//Draw mesh

	glm::mat4 modelM = glm::mat4(1.0f);

	modelM = glm::translate(modelM, transform.pos);
	modelM = glm::scale(modelM, transform.dim);
	//modelM = glm::rotate(modelM, transform.rot.y, glm::vec3(0.0f, 1.0f, 1.0f));

	shader.setMat4f("model", modelM);
	if (verticesSize / sizeof(float) / 3 <= 2)
	{
		glDrawArraysInstanced(GL_LINES, 0, (verticesSize / sizeof(float)), count);
	}
	else if (indicesSize > 0)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indicesSize / sizeof(float), GL_UNSIGNED_INT, 0, count);
	}
	else
	{
		glDrawArraysInstanced(GL_TRIANGLES, 0, (verticesSize / sizeof(float)), count);
	}
	glDeleteBuffers(1, &instanceVBO);
	glBindVertexArray(0);
}

void Object::Draw(Shader& shader) {
	shader.use();
	//Draw mesh
	//glBindTexture(GL_TEXTURE_2D, texture != nullptr && texture > 0? *texture : 0);
	glBindVertexArray(VAO);

	glm::mat4 modelM = glm::mat4(1.0f);

	modelM = glm::translate(modelM, transform.pos);
	modelM = glm::scale(modelM, transform.dim);
	//modelM = glm::rotate(modelM, transform.rot.y, glm::vec3(0.0f, 1.0f, 1.0f));

	shader.setMat4f("model", modelM);
	if (verticesSize / sizeof(float) / 3.0f <= 2.0f)
	{
		glDrawArrays(GL_LINES, 0, (verticesSize / sizeof(float)));
	}
	else if (indicesSize > 0)
	{
		glDrawElements(GL_TRIANGLES, indicesSize / sizeof(float), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (verticesSize / sizeof(float)));
	}
	glBindVertexArray(0);
}