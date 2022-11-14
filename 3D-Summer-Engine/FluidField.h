#pragma once
#include <iostream>

#include "object_includes.h"
#include "Line.h"

#include "Shader.h"
#include "Framebuffer.h"
#include "Texture2D.h"
#include "Compute.h"

#include <GLFW/glfw3.h>

class FluidField
{
	struct DoubleFramebuffer
	{
	private:
		Framebuffer* fb1;	//Read
		Framebuffer* fb2;	//Write
	public:
		DoubleFramebuffer(unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param) {
			fb1 = new Framebuffer(width, height, internalFormat, format, type, param);
			fb2 = new Framebuffer(width, height, internalFormat, format, type, param);
		}

		~DoubleFramebuffer() {
			delete(fb1);
			delete(fb2);
		}

		//Get the currently bound buffer which has the purpose of being read from.
		Framebuffer* readBuffer() {
			return fb1;
		}

		void readBuffer(Framebuffer* buffer) {
			fb1 = buffer;
		}

		//Get the currently bound buffer which has the purpose of being written to.
		Framebuffer* writeBuffer() {
			return fb2;
		}

		void writeBuffer(Framebuffer* buffer) {
			fb2 = buffer;
		}

		//Swap buffers so that the write buffer becomes the readbuffer or vice versa
		void swap() {
			Framebuffer* temp = fb1;
			fb1 = fb2;
			fb2 = temp;
		}
	};

	struct TexFormat
	{
		const GLint internal;
		const GLenum format;
		TexFormat(GLint internalFormat, GLenum format) : internal(internalFormat), format(format){}
	};

private:
	const char* p_COMPUTE_SHADER						 = "Shaders/compute_shader_backup.glsl";
	const char* p_advection_shader						 = "Shaders/compute_advection_shader.frag";
	const char* p_jacobi_shader							 = "Shaders/compute_jacobi_shader.frag";
	const char* p_force_shader							 = "Shaders/compute_force_shader.frag";
	const char* p_divergence_shader						 = "Shaders/compute_divergence_shader.frag";
	const char* p_gradient_subtraction_shader			 = "Shaders/compute_gradient_subtraction_shader.frag";
	const char* p_bounds_shader							 = "Shaders/compute_bounds_shader.frag";

	const char* p_VISUALISE_GRID_VERTEX_SHADER			 = "Shaders/visualise_grid_vertex_shader.vert";
	const char* p_VISUALISE_GRID_FRAGMENT_SHADER		 = "Shaders/visualise_grid_fragment_shader.frag";
	const char* p_VERTEX_SHADER							 = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER						 = "Shaders/fragment_shader.frag";
	const char* p_TEXTURE								 = "C:/Users/to9751/Pictures/Generated Images/Multicolored_pattern.png";

	//Framebuffers
	DoubleFramebuffer* m_dye_buffer;
	DoubleFramebuffer* m_velocity_buffer;
	Framebuffer* m_divergence_buffer;
	Framebuffer* m_curl_buffer;
	DoubleFramebuffer* m_pressure_buffer;

	Compute* m_compute_shader;
	Shader* m_compute_temp;
	Shader m_advection_shader;
	Shader m_jacobi_iteration_shader; //Diffusion
	Shader m_force_shader;
	Shader m_divergence_shader;
	Shader m_gradient_subtraction_shader;
	Shader m_bounds_shader;

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
	const float	 m_timestep								 = 1;

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
		m_HEIGHT(HEIGHT),
		m_advection_shader(p_VERTEX_SHADER, p_advection_shader),
		m_jacobi_iteration_shader(p_VERTEX_SHADER, p_jacobi_shader),
		m_force_shader(p_VERTEX_SHADER, p_force_shader),
		m_divergence_shader(p_VERTEX_SHADER, p_divergence_shader),
		m_gradient_subtraction_shader(p_VERTEX_SHADER, p_gradient_subtraction_shader),
		m_bounds_shader(p_VERTEX_SHADER, p_bounds_shader)
	{
		std::cout << "INITIALIZING::FLUIDFIELD" << std::endl;
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
		m_translations.resize(resolution);		//reserve memory
		Init();

		GLenum textureType = GL_UNSIGNED_BYTE;
		TexFormat rgba(GL_RGBA32F, GL_RGBA);
		TexFormat rg(GL_RG32F, GL_RG);
		TexFormat r(GL_R32F, GL_RED);

		// Dye
		m_dye_buffer = new DoubleFramebuffer(m_fieldWidth, m_fieldWidth, rgba.internal, rgba.format, textureType, GL_NEAREST);
		// Velocity
		m_velocity_buffer = new DoubleFramebuffer(m_fieldWidth, m_fieldWidth, rg.internal, rg.format, textureType, GL_NEAREST);
		// Divergence
		m_divergence_buffer = new Framebuffer(m_fieldWidth, m_fieldWidth, r.internal, r.format, textureType, GL_NEAREST);
		// Curl
		m_curl_buffer = new Framebuffer(m_fieldWidth, m_fieldWidth, r.internal, r.format, textureType, GL_NEAREST);
		// Pressure
		m_pressure_buffer = new DoubleFramebuffer(m_fieldWidth, m_fieldWidth, r.internal, r.format, textureType, GL_NEAREST);

		std::cout << "SUCCESS::INITIALIZATION::FLUIDFIELD" << std::endl;
	}

	~FluidField() {
		delete(m_dye_buffer);
		delete(m_velocity_buffer);
		delete(m_divergence_buffer);
		delete(m_curl_buffer);
		delete(m_pressure_buffer);

		delete(m_primary_shader);
		delete(m_visualise_grid_shader);
		delete(m_compute_shader);
		delete(m_fieldQuad);
		delete(m_quad);
		delete(m_line);
		std::cout << "DESTROYED::FLUIDFIELD" << std::endl;
	}

	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Visualize the Cell Field
	void DrawCellField(glm::vec3 origin);
	//Set mouse position and button properties
	void updateMouse(double* mouseX, double* mouseY, bool* mouse_down);
	//Move forward in time, update values
	void timeStep(float dt); 

private:
	void initFramebuffers();
	//Draw the fluid to the targeted framebuffer
	void blit(Framebuffer* target, Shader* shader);
	//Add a splat of velocity in the specified position
	void addDye(glm::vec2 pos, float amount);
	//Clear everything and start from the beginning
	void reset();
};

