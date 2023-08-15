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

	struct TexelSize
	{
		float x, y;
	};

public:
	unsigned int fbo, texture;
	unsigned int width, height, resolution;
	TexelSize texelSize;

	Framebuffer(unsigned int res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	~Framebuffer()
	{
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);
		std::cout << "DESTROYED::FRAMEBUFFER" << std::endl;
	}

	void setDimensions(unsigned int width, unsigned int height, unsigned int resolution);
	void bind();
	int setTexture(unsigned int id);
	void setTextureSource(const char* path, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	int status();

private:
	void calculateTexelsize(unsigned int width, unsigned int height, unsigned int resolution);
};