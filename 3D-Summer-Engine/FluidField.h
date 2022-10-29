#pragma once
#include <iostream>

#include "object_includes.h"
#include "Line.h"

#include "Shader.h"
#include "Texture2D.h"
#include "Compute.h"

class FluidField
{
private:
	const char* p_COMPUTE_SHADER						 = "Shaders/compute_shader.glsl";
	const char* p_VISUALISE_GRID_VERTEX_SHADER			 = "Shaders/visualise_grid_vertex_shader.vert";
	const char* p_VISUALISE_GRID_FRAGMENT_SHADER		 = "Shaders/visualise_grid_fragment_shader.frag";
	const char* p_VERTEX_SHADER							 = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER						 = "Shaders/fragment_shader.frag";
	const char* p_TEXTURE								 = "C:/Users/to9751/Pictures/Generated Images/Multicolored_pattern.png";

	Compute* m_compute_shader;
	Shader* m_shader;
	Shader* m_visualise_grid_shader;

	//Field
	Texture2D*	 m_texture;
	Rect*		 m_fieldQuad;
	const float	 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	//Visualisation
	bool					m_showDataVectors;
	std::vector<glm::vec2>	m_translations;
	Rect*					m_quad;
	Line*					m_line;

	//Initialize the grid
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution): 
		m_resolution(resolution),
		m_fieldWidth(sqrt(resolution)),
		m_WIDTH(WIDTH),
		m_HEIGHT(HEIGHT)
	{
		m_shader = new Shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
		m_visualise_grid_shader = new Shader(p_VISUALISE_GRID_VERTEX_SHADER, p_VISUALISE_GRID_FRAGMENT_SHADER);
		m_texture = new Texture2D(p_TEXTURE);
		m_quad = new Rect();
		m_fieldQuad = new Rect(glm::vec3(m_WIDTH / 2.f, m_HEIGHT / 2.f, 0.0f), glm::vec3(m_WIDTH, m_HEIGHT, 0.0f), glm::vec3(0.0f), m_texture->get());
		//m_line = new Line(0.0f, 0.0f, 0.0f, 0.0f);
		m_translations.resize(resolution);
		Init();
	}

	~FluidField() {
		glDeleteProgram(m_shader->getID());
		glDeleteProgram(m_visualise_grid_shader->getID());
		glDeleteTextures(1, m_texture->get());
		delete(m_shader);
		delete(m_visualise_grid_shader);
		delete(m_fieldQuad);
		delete(m_quad);
		delete(m_line);
	}

	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Visualize the Cell Field
	void DrawCellField(glm::vec3 origin);

	//Move forward in time, update values
	void timeStep(); 
	void addDye(glm::vec2 pos, float amount);
	//Clear everything and start from the beginning
	void reset();
};

