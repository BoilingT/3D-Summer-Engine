#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vShader, fShader;
	int success;
	char compileInfo[512];
	std::string vShaderContent, fShaderContent;

	//Read shader files
	fileHandler.readFile(vertexPath, vShaderContent);
	fileHandler.readFile(fragmentPath, fShaderContent);
	
	std::cout << "Vertex Shader:\n" << vShaderContent << "\nEND OF FILE" << std::endl;
	std::cout << "Fragment Shader:\n" << fShaderContent << "\nEND OF FILE" << std::endl;

	const char* vShaderSource = vShaderContent.c_str();
	const char* fShaderSource = fShaderContent.c_str();

	//Create each shader
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Compile Vertex Shader
	glShaderSource(vShader, 1, &vShaderSource, NULL);
	glCompileShader(vShader);

	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vShader, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << compileInfo << std::endl;
	}

	//Compile Fragment Shader
	glShaderSource(fShader, 1, &fShaderSource, NULL);
	glCompileShader(fShader);
	
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fShader, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << compileInfo << std::endl;
	}

	//Create and link Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vShader);
	glAttachShader(ID, fShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, compileInfo);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << compileInfo << std::endl;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

unsigned int Shader::getID() {
	return ID;
}