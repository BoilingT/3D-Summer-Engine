#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>

class Framebuffer
{
public:
	unsigned int fbo;
	unsigned int texture;
	unsigned int width, height;
	float resolution;
	float texelSizeX, texelSizeY;
	
	Framebuffer(float res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	~Framebuffer() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);
		std::cout << "DESTROYED::FRAMEBUFFER" << std::endl;
	}

	void updateDimensions(unsigned int width, unsigned int height);
	void bind();
	int setTexture(unsigned int id);
	void setTextureSource(const char* path, int width, int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	int status();
};