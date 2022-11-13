#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vShader, fShader;
	int success;
	char compileInfo[512];
	std::string vShaderContent, fShaderContent;

	//Read shader files
	fileHandler.readFile(vertexPath, vShaderContent);
	fileHandler.readFile(fragmentPath, fShaderContent);
	
	//std::cout << "Vertex Shader:\n" << vShaderContent << "\nEND OF FILE" << std::endl;
	//std::cout << "Fragment Shader:\n" << fShaderContent << "\nEND OF FILE" << std::endl;

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
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << "FRAGMENT::SHADER::" << fragmentPath << "\n" << "VERTEX::SHADER::" << vertexPath << "\n" << compileInfo << std::endl;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

Shader::Shader(const char* shaderPath, GLenum type) {
	unsigned int shader;
	int success;
	char compileInfo[512];
	std::string shaderContent;

	//Read shader files
	fileHandler.readFile(shaderPath, shaderContent);

	//std::cout << "Vertex Shader:\n" << vShaderContent << "\nEND OF FILE" << std::endl;
	//std::cout << "Fragment Shader:\n" << fShaderContent << "\nEND OF FILE" << std::endl;

	const char* shaderSource = shaderContent.c_str();

	//Create each shader
	shader = glCreateShader(type);

	//Compile Shader
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::COMPILATION::FAILED\n" << "SHADER:: " << shaderPath << "\n" << compileInfo << std::endl;
		return;
	}

	//Create and link Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, compileInfo);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << "SHADER:: " << shaderPath << "\n" << compileInfo << std::endl;
	}

	glDeleteShader(shader);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
	glDeleteTextures(1, &m_compute_texture);
}

void Shader::generateTexture(unsigned int TEXTURE_WIDTH, unsigned int TEXTURE_HEIGHT)
{
	glGenTextures(1, &m_compute_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_compute_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, m_compute_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}

void Shader::setValues(float* values, int width, int height) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, values);
}

unsigned int* Shader::getTexture()
{
	return &m_compute_texture;
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

void Shader::setMat4f(const std::string name, glm::mat4 values) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(values));
}

unsigned int Shader::getID() {
	return ID;
}
