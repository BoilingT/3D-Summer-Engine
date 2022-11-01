#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h> //Needs to be included before GLFW


class Compute
{
private:
	unsigned int m_ID;
	glm::vec2 m_workSize;
	Shader* m_computeShader;
	unsigned int m_out_texture;

	void createTexture(glm::vec2 size) {
		//Generate and activate textures
		glGenTextures(1, &m_out_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_out_texture);

		//Set paramaters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Create empty 2D textures
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_workSize.x, m_workSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindImageTexture(0, m_out_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}
public:
	Compute(const char* path, glm::vec2 size) {
		m_computeShader = new Shader(path, GL_COMPUTE_SHADER);
		m_ID = m_computeShader->getID();
		m_workSize = size;
		createTexture(size);
	}

	~Compute() {
		delete(m_computeShader);
		m_computeShader = NULL;
	}

	//Setup class in order to use the program
	void use() {
		m_computeShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_out_texture);
	}

	//Start the compute shader with the given work group size
	void dispatch() {
		glDispatchCompute(m_workSize.x, m_workSize.y, 1);
	}

	//Wait until the shader program is done processing
	void wait() {
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	//Set compute shader values
	void setValues(void* values) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_workSize.x, m_workSize.y, 0, GL_RED, GL_FLOAT, values);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_workSize.x, m_workSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, values);
	}

	std::vector<float> get_values() {
		unsigned int collection_size = m_workSize.x * m_workSize.y;
		std::vector<float> compute_data(collection_size);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, compute_data.data());
		return compute_data;
	}

	GLuint* getTexture() {
		return &m_out_texture;
	}

	unsigned int getID() {
		return m_ID;
	}

	Shader* getShader() {
		return m_computeShader;
	}
};

