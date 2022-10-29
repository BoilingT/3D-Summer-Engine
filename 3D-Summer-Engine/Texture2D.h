#pragma once
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"

class Texture2D
{
private:
	GLuint m_texture;
	int m_width, m_height, m_nrChannels;
public:
	Texture2D(const char* filepath);
	GLuint* get();
	int height();
	int width();
	int nrChannels();

	void use();
};

