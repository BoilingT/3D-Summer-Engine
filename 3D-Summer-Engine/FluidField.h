#pragma once
#include <iostream>

#include "object_includes.h"
#include "Line.h"

#include "Shader.h"
#include "Texture2D.h"
#include "Compute.h"

#include <GLFW/glfw3.h>

class FluidField
{
private:
	const char* p_COMPUTE_SHADER						 = "Shaders/compute_shader_backup.glsl";
	const char* p_advection_shader						 = "Shaders/compute.glsl";
	const char* p_diffusion_shader						 = "Shaders/compute.glsl";
	const char* p_force_shader							 = "Shaders/compute.glsl";
	const char* p_divergence_shader						 = "Shaders/compute.glsl";
	const char* p_gradient_subtraction_shader			 = "Shaders/compute.glsl";
	const char* p_bounds_shader							 = "Shaders/compute.glsl";

	const char* p_VISUALISE_GRID_VERTEX_SHADER			 = "Shaders/visualise_grid_vertex_shader.vert";
	const char* p_VISUALISE_GRID_FRAGMENT_SHADER		 = "Shaders/visualise_grid_fragment_shader.frag";
	const char* p_VERTEX_SHADER							 = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER						 = "Shaders/fragment_shader.frag";
	const char* p_TEXTURE								 = "C:/Users/to9751/Pictures/Generated Images/Multicolored_pattern.png";

	Compute* m_compute_shader;

	Compute* m_advection_shader;
	Compute* m_diffusion_shader;
	Compute* m_force_shader;
	Compute* m_divergence_shader;
	Compute* m_gradient_subtraction_shader;
	Compute* m_bounds_shader;

	Shader* m_primary_shader;
	Shader* m_visualise_grid_shader;

	//Field
	//unsigned const int m_COMPUTE_TEXTURE_WIDTH;
	//unsigned const int m_COMPUTE_TEXTURE_HEIGHT;

	Texture2D*	 m_texture;
	unsigned int texture;
	Texture2D*	 m_texture_buffer;
	Rect*		 m_fieldQuad;
	const float	 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	//Visualisation
	bool					m_showDataVectors;
	std::vector<glm::vec2>	m_translations;
	Rect*					m_quad;
	Line*					m_line;

	double m_prevMouseX									 = 0;
	double m_prevMouseY									 = 0;
	double m_mouseTravelX								 = 0;
	double m_mouseTravelY								 = 0;
	bool m_mouse_down									 = false;
	bool m_mouse_released								 = true;

	//Initialize the grid
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution): 
		m_resolution(resolution),
		m_fieldWidth(sqrt(resolution)),
		m_WIDTH(WIDTH),
		m_HEIGHT(HEIGHT)
	{
		m_primary_shader					 = new Shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
		m_visualise_grid_shader				 = new Shader(p_VISUALISE_GRID_VERTEX_SHADER, p_VISUALISE_GRID_FRAGMENT_SHADER);
		m_texture							 = new Texture2D(p_TEXTURE);
		m_texture_buffer					 = new Texture2D();
		m_quad								 = new Rect();
		m_fieldQuad							 = new Rect(
												glm::vec3(m_WIDTH / 2.f, m_HEIGHT / 2.f, 0.0f), 
												glm::vec3(m_WIDTH, m_HEIGHT, 0.0f), 
												glm::vec3(0.0f), 
												m_texture->get());
		m_translations.resize(resolution);
		Init();
	}

	~FluidField() {
		glDeleteProgram(m_primary_shader->getID());
		glDeleteProgram(m_visualise_grid_shader->getID());
		glDeleteTextures(1, m_texture->get());
		glDeleteTextures(1, m_texture_buffer->get());
		delete(m_primary_shader);
		delete(m_visualise_grid_shader);
		delete(m_compute_shader);
		delete(m_fieldQuad);
		delete(m_quad);
		delete(m_line);
	}

	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Visualize the Cell Field
	void DrawCellField(glm::vec3 origin);

	void updateMouse(double* mouseX, double* mouseY, bool* mouse_down);
	//Move forward in time, update values
	void timeStep(); 
	void addDye(glm::vec2 pos, float amount);
	//Clear everything and start from the beginning
	void reset();
};

