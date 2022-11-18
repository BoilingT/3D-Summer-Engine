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
	struct Mouse {
		double height, width;
		Mouse(double _width, double _height) {
			width = _width;
			height = _height;
		}
		bool down = false;
		glm::vec2 texcoord_travel = glm::vec2(0.0f);
		glm::vec2 texcoord_pos = glm::vec2(0.0f);
		glm::vec2 prev_texcoord_pos = glm::vec2(0.0f);
		glm::vec2 texcoord_delta = glm::vec2(0.0f);

		glm::vec2 window_travel = glm::vec2(0.0f);
		glm::vec2 window_pos = glm::vec2(0.0f);
		glm::vec2 prev_window_pos = glm::vec2(0.0f);
		glm::vec2 window_delta = glm::vec2(0.0f);

		void update(double mouseX, double mouseY, bool mouse_is_down) {
			window_delta = glm::vec2(mouseX, mouseY) - prev_window_pos;
			prev_window_pos = window_pos;
			window_pos = glm::vec2(mouseX, mouseY);
			window_travel += window_travel;

			texcoord_delta = texcoord_pos - prev_texcoord_pos;
			prev_texcoord_pos = texcoord_pos;
			texcoord_pos = glm::vec2(mouseX / width, 1.f - mouseY / height);
			texcoord_travel += texcoord_travel;
			down = mouse_is_down;
			if (!mouse_is_down)
			{
				texcoord_travel = glm::vec2(0.0f);
				window_travel = glm::vec2(0.0f);
			}
		}
	};

	struct DoubleFramebuffer
	{
	private:
		Framebuffer* fb1;	//Read
		Framebuffer* fb2;	//Write
	public:
		DoubleFramebuffer(float res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param) {
			fb1 = new Framebuffer(res, width, height, internalFormat, format, type, param);
			fb2 = new Framebuffer(res, width, height, internalFormat, format, type, param);
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
	const char* p_clear_shader							 = "Shaders/compute_clear_shader.frag";
	const char* p_gradient_subtraction_shader			 = "Shaders/compute_gradient_subtraction_shader.frag";
	const char* p_bounds_shader							 = "Shaders/compute_bounds_shader.frag";
	const char* p_splat_shader							 = "Shaders/compute_splat_shader.frag";

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
	Framebuffer* m_current_buffer;
	unsigned int* m_current_read_buffer;

	Shader m_advection_shader;
	Shader m_jacobi_iteration_shader; //Diffusion
	Shader m_force_shader;
	Shader m_divergence_shader;
	Shader m_clear_shader;
	Shader m_gradient_subtraction_shader;
	Shader m_bounds_shader;
	Shader m_splat_shader;

	Shader* m_primary_shader;
	Shader* m_visualise_grid_shader;

	Texture2D*	 m_texture;
	unsigned int texture;
	Texture2D*	 m_texture_buffer;
	Rect*		 m_fieldQuad;
	const float	 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	const float	 m_dye_force							 = 5000.0f;
	const float	 m_dye_radius							 = 0.15f;
	const float	 m_dye_dissipation						 = 0.3f;
	const float	 m_velocity_dissipation					 = 0.4f;
	const int	 m_diffuseIterations					 = 40;
	const float	 m_viscosity							 = 0.0f;
	const int	 m_pressureIterations					 = 40;
	const float	 m_pressure								 = 0.8f;
	const float	 m_timestep								 = 1.f;

	//Visualisation
	bool					m_showDataVectors;
	std::vector<glm::vec2>	m_translations;
	Rect*					m_quad;
	Line*					m_line;

	Mouse  m_mouse;

	//Initialize the grid
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution) :
		m_resolution(resolution),
		m_fieldWidth(sqrt(resolution)),
		m_WIDTH(WIDTH),
		m_HEIGHT(HEIGHT),
		m_advection_shader(p_VERTEX_SHADER, p_advection_shader),
		m_jacobi_iteration_shader(p_VERTEX_SHADER, p_jacobi_shader),
		m_force_shader(p_VERTEX_SHADER, p_force_shader),
		m_divergence_shader(p_VERTEX_SHADER, p_divergence_shader),
		m_clear_shader(p_VERTEX_SHADER, p_clear_shader),
		m_gradient_subtraction_shader(p_VERTEX_SHADER, p_gradient_subtraction_shader),
		m_bounds_shader(p_VERTEX_SHADER, p_bounds_shader),
		m_splat_shader(p_VERTEX_SHADER, p_splat_shader),
		m_mouse(WIDTH, HEIGHT)
	{
		std::cout << "INITIALIZING::FLUIDFIELD" << std::endl;
		m_primary_shader					 = new Shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
		m_visualise_grid_shader				 = new Shader(p_VISUALISE_GRID_VERTEX_SHADER, p_VISUALISE_GRID_FRAGMENT_SHADER);
		m_texture							 = new Texture2D(p_TEXTURE);
		m_texture_buffer					 = new Texture2D();
		m_quad								 = new Rect();
		m_fieldQuad							 = new Rect(
												glm::vec3(0.0f), 
												glm::vec3(0.0f), 
												glm::vec3(0.0f), 
												m_texture->get());
		m_translations.resize(resolution);		//reserve memory
		Init();

		GLenum textureType = GL_UNSIGNED_BYTE;
		TexFormat rgba(GL_RGBA32F, GL_RGBA);
		TexFormat rg(GL_RG32F, GL_RG);
		TexFormat r(GL_R32F, GL_RED);
		glDisable(GL_BLEND);
		// Dye
		m_dye_buffer = new DoubleFramebuffer(m_resolution*2, m_resolution, m_resolution, rgba.internal, rgba.format, textureType, GL_LINEAR);
		// Velocity
		m_velocity_buffer = new DoubleFramebuffer(m_resolution, m_resolution, m_resolution, rg.internal, rg.format, textureType, GL_LINEAR);
		// Divergence
		m_divergence_buffer = new Framebuffer(m_resolution, m_resolution, m_resolution, r.internal, r.format, textureType, GL_NEAREST);
		// Curl
		m_curl_buffer = new Framebuffer(m_resolution, m_resolution, m_resolution, r.internal, r.format, textureType, GL_NEAREST);
		// Pressure
		m_pressure_buffer = new DoubleFramebuffer(m_resolution, m_resolution, m_resolution, r.internal, r.format, textureType, GL_NEAREST);

		m_current_read_buffer = &m_dye_buffer->readBuffer()->texture;
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
	void swapBuffer(int i) {
		if (i == 1)
		{
			std::cout << "BUFFER::DYE" << std::endl;
			m_current_buffer = 0;
		}
		else if (i == 2)
		{
			std::cout << "BUFFER::VELOCITY" << std::endl;
			//m_current_read_buffer = &m_velocity_buffer->readBuffer()->texture;
			m_current_buffer = m_velocity_buffer->readBuffer();
		}
		else if(i == 3){
			std::cout << "BUFFER::DIVERGENCE" << std::endl;
			//m_current_read_buffer = &m_divergence_buffer->texture;
			m_current_buffer = m_divergence_buffer;

		}
		else if (i == 4) {
			std::cout << "BUFFER::PRESSURE" << std::endl;
			//m_current_read_buffer = &m_pressure_buffer->readBuffer()->texture;
			m_current_buffer = m_pressure_buffer->readBuffer();

		}
		else if (i == 5) {
			std::cout << "BUFFER::CURL" << std::endl;
			//m_current_read_buffer = &m_curl_buffer->texture;
			m_current_buffer = m_curl_buffer;
		}
	}

private:
	//Draw the fluid to the targeted framebuffer
	void blit(Framebuffer* target, Shader* shader);
	void advect(float dt);
	void diffuse(float dt);
	void addForces(float dt);
	void project(float dt);
	void gradientSubtract(float dt);
	void divergence(float dt);
	//Add a splat of velocity in the specified position
	void splat(glm::vec2 pos, float r);
	//Clear everything and start from the beginning
	void reset();
};

