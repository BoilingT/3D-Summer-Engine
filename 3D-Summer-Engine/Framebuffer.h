#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

class Framebuffer
{
public:
	unsigned int fbo;
	unsigned int texture;
	unsigned int width, height;
	
	Framebuffer(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param);
	~Framebuffer() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &texture);
		std::cout << "DESTROYED::FRAMEBUFFER" << std::endl;
	}

	void bind();
	int status();
};