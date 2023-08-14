#include "Texture2D.h"

Texture2D::Texture2D()
{
	m_data = 0;
	m_texture = 0;
}

Texture2D::Texture2D( const char *filepath )
{
	stbi_set_flip_vertically_on_load( true );
	m_data = 0;
	m_texture = 0;
	glGenTextures( 1, &m_texture );
	setParameters();

	m_data = stbi_load( filepath, &m_width, &m_height, &m_nrChannels, 0 );
	generateTexture( m_data, m_width, m_height );
}

Texture2D::Texture2D( unsigned char *data, int width, int height )
{
	m_data = 0;
	m_texture = 0;
	glGenTextures( 1, &m_texture );
	setParameters();
	generateTexture( data, width, height );
}

void Texture2D::use()
{
	glBindTexture( GL_TEXTURE_2D, m_texture );
}

void Texture2D::generateFrom( const char *filepath )
{
	m_data = stbi_load( filepath, &m_width, &m_height, &m_nrChannels, 0 );
}

void Texture2D::setParameters()
{
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void Texture2D::generateTexture( unsigned char *data, int width, int height )
{
	if ( data )
	{
		m_data = data;
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		std::cout << "ERROR::TEXTURE::2D::GENERATION::FAILURE" << std::endl;
	}
}

GLuint *Texture2D::get()
{
	return &m_texture;
}

unsigned char *Texture2D::data()
{
	return m_data;
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