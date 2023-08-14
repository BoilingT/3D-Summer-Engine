#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>

class Framebuffer
{
private:
	GLint internalFormat, param;
	GLenum format, type;
public:
	unsigned int fbo;
	unsigned int texture;
	unsigned int width, height;
	float resolution;
	float texelSizeX, texelSizeY;

	Framebuffer(float res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	~Framebuffer()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);
		std::cout << "DESTROYED::FRAMEBUFFER" << std::endl;
	}

	void setDimensions(unsigned int width, unsigned int height, float resolution);
	void bind();
	int setTexture(unsigned int id);
	void setTextureSource(const char *path, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	int status();

private:
	void calculateTexelsize(float width, float height, float resolution);
};