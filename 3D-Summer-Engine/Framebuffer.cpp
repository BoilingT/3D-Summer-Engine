#include "Framebuffer.h"

Framebuffer::Framebuffer(float res, unsigned int w, unsigned int h, GLint internalFormat, GLenum format, GLenum type, GLint param) {
	resolution = res;
	width = w;
	height = h;
	float ratio = w / h;
	if (ratio < 1)
	{
		ratio = 1 / ratio;
	}
	float min = round(resolution);
	float max = round(resolution * ratio);
	if (w > h)
	{
		width = max;
		height = min;
	}
	else {
		width = min;
		height = max;
	}

	texelSizeX = 1.f / width;
	texelSizeY = 1.f / height;

	glActiveTexture(GL_TEXTURE0);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	GLuint clearColor[4] = { 0, 0, 0, 0 };
	glClearBufferuiv(GL_COLOR, fbo, clearColor);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Framebuffer::status() {
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

int Framebuffer::setTexture(unsigned int id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
	return id;
}
