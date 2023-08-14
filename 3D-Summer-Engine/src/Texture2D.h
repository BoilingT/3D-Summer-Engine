#pragma once
#include <iostream>
#include <glad/glad.h>
#include <stb/stb_image.h>

class Texture2D
{
private:
	GLuint m_texture;
	int m_width, m_height, m_nrChannels;
	unsigned char *m_data;
public:
	Texture2D();
	Texture2D(const char *filepath);
	Texture2D(unsigned char *data, int width, int height);
	~Texture2D() { stbi_image_free(m_data); }

	GLuint *get();
	unsigned char *data();
	int height();
	int width();
	int nrChannels();

	void use();
	void generateFrom(const char *filepath);
private:
	void setParameters();
	void generateTexture(unsigned char *data, int width, int height);

};

