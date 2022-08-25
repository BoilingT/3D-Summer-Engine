#pragma once
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "fileHandler.h"

class Shader
{
private:
	unsigned int ID;
	FileHandler fileHandler;

public:
	//Read, Compile, Link
	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setMat4f(const std::string name, glm::mat4 values) const;
	unsigned int getID();
};

