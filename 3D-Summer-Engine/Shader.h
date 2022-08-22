#pragma once
#include <iostream>
#include <string>

class Shader
{
private:
	unsigned int ID;
public:
	//Read, Compile, Link
	Shader();

	void use();
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
};

