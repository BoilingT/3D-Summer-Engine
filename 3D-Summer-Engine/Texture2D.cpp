#include "Texture2D.h"

Texture2D::Texture2D(const char* filepath) {
	//stbi_set_flip_vertically_on_load(true);
	m_texture = 0;
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(filepath, &m_width, &m_height, &m_nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::TEXTURE::2D::LOAD::FAILURE" << std::endl;
	}
	stbi_image_free(data);
}

void Texture2D::use() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

GLuint* Texture2D::get()
{
	return &m_texture;
}

int Texture2D::height()
{
	return m_height;
}

int Texture2D::width()
{
	return m_width;
}

int Texture2D::nrChannels()
{
	return m_nrChannels;
}