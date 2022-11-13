#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int w, unsigned int h, GLint internalFormat, GLenum format, GLenum type, GLint param) {
	width = w;
	height = h;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, NULL);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

int Framebuffer::status() {
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}