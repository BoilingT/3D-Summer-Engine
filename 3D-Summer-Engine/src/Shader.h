#pragma once
#include <iostream>
#include <vector>
#include "glm_includes.h"
#include <string>
#include <glad/glad.h>
#include <unordered_map>
#include "fileHandler.h"

class Shader
{
private:
	FileHandler fileHandler;
	unsigned int m_compute_texture;
	unsigned int ID;

public:
	std::unordered_map<std::string, char> uniforms;
	//Read, Compile, Link
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* shaderPath, GLenum type);
	~Shader();

	void use();
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setMat4f(const std::string name, glm::mat4 values) const;
	int getUniformLocation(const std::string name);
	unsigned int getID();

	void generateTexture(unsigned int TEXTURE_WIDTH, unsigned int TEXTURE_HEIGHT);
	void setValues(float* values, int width, int height);
	unsigned int* getTexture();

private:
	void getUniforms();
};
