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
		bool down = false;
		double height, width;

		glm::vec2 texcoord_travel	 = glm::vec2(0.0f);
		glm::vec2 texcoord_pos		 = glm::vec2(0.0f);
		glm::vec2 prev_texcoord_pos	 = glm::vec2(0.0f);
		glm::vec2 texcoord_delta	 = glm::vec2(0.0f);

		glm::vec2 window_travel		 = glm::vec2(0.0f);
		glm::vec2 window_pos		 = glm::vec2(0.0f);
		glm::vec2 prev_window_pos	 = glm::vec2(0.0f);
		glm::vec2 window_delta		 = glm::vec2(0.0f);

		Mouse(double _width, double _height) {
			width = _width;
			height = _height;
		}

		void update(double mouseX, double mouseY, bool mouse_is_down) {
			window_delta			 = glm::vec2(mouseX, mouseY) - prev_window_pos;
			prev_window_pos			 = window_pos;
			window_pos				 = glm::vec2(mouseX, mouseY);
			window_travel			 += window_travel;

			texcoord_delta			 = texcoord_pos - prev_texcoord_pos;
			prev_texcoord_pos		 = texcoord_pos;
			texcoord_pos			 = glm::vec2(mouseX / width, 1.f - mouseY / height);
			texcoord_travel			 += texcoord_travel;
			down					 = mouse_is_down;

			if (!mouse_is_down)
			{
				texcoord_travel		 = glm::vec2(0.0f);
				window_travel		 = glm::vec2(0.0f);
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

		//Set the currently bound buffer for reading to another specified buffer
		void readBuffer(Framebuffer* buffer) {
			fb1 = buffer;
		}

		//Get the currently bound buffer which has the purpose of being written to.
		Framebuffer* writeBuffer() {
			return fb2;
		}

		//Set the currently bound writing for reading to another specified buffer
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
		const GLint internal; //Type of data to be stored
		const GLenum format; //Source data type
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
	const char* p_vorticity_shader						 = "Shaders/compute_vorticity_shader.frag";
	const char* p_bounds_shader							 = "Shaders/compute_bounds_shader.frag";
	const char* p_splat_shader							 = "Shaders/compute_splat_shader.frag";

	const char* p_VISUALISE_GRID_VERTEX_SHADER			 = "Shaders/visualise_grid_vertex_shader.vert";
	const char* p_VISUALISE_GRID_FRAGMENT_SHADER		 = "Shaders/visualise_grid_fragment_shader.frag";
	const char* p_VERTEX_SHADER							 = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER						 = "Shaders/fragment_shader.frag";
	const char* p_TEXTURE								 = "Images/Generated Images/notGenerated.jpg";

	//Framebuffers
	DoubleFramebuffer* m_velocity_buffer;				//Contains velocities to be advected
	Framebuffer* m_divergence_buffer;					//Contains divergent velocities
	DoubleFramebuffer* m_pressure_buffer;				//Contains a pressure field

	//Application buffers
	DoubleFramebuffer* m_dye_buffer;					//Contains dye quantities to be advected
	DoubleFramebuffer* m_temperature_buffer;			//Boyancy and Convection
	DoubleFramebuffer* m_vorticity_buffer;
	Framebuffer* m_curl_buffer;							//Contains curling velocities
	//Screen rendering buffers
	Framebuffer* m_current_buffer;						//Contains the buffer that will be used when rendering

	//Shaders
	Shader m_advection_shader;							//Used for advecting quantities in the fluid dependent on the given velocity buffer
	Shader m_jacobi_iteration_shader;					//Used for Pressure and Diffusion
	Shader m_force_shader;								//TODO: Used for application of force (Velocities)
	Shader m_divergence_shader;							//Calculates change in density (Density velocities)
	Shader m_clear_shader;								//Used for clearing a buffer of its values
	Shader m_gradient_subtraction_shader;				//Subtract a gradient from given buffer
	Shader m_vorticity_shader;							//TODO
	Shader m_bounds_shader;								//TODO: Control of Fluid boundaries
	Shader m_splat_shader;								//Used for application of Dye and Velocity manipulation
	//Screen rendering shaders
	Shader* m_primary_shader;							//Used to render to the screen
	Shader* m_visualise_grid_shader;					//TODO: Used to render a visual representation of the resolution used to the screen

	Texture2D*	 m_texture;
	unsigned int texture;
	Texture2D*	 m_texture_buffer;
	Rect*		 m_fieldQuad;
	const float	 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	const float	 m_dye_force							 = 6000.0f;		// Force used to create velocities
	const float	 m_dye_radius							 = 0.5f;		// Radius of the applicable dye and velocites
	const float	 m_dye_dissipation						 = 0.3f;		// The rate at which the dye clears from the screen
	const float	 m_velocity_dissipation					 = 0.3f;		// The rate at which the velocities reduces to zero
	const int	 m_diffuseIterations					 = 30;			// Number of iterations used to calculate proper diffusion of the applied dye or velocities
	const float	 m_viscosity							 = 0.3f;		// Drag factor of the fluid
	const int	 m_pressureIterations					 = 80;			// Number of iterations used to calculate more precise pressure fields
	const float	 m_pressure_dissipation					 = 0.8f;		// The rate at which the pressure field is cleared
	const float	 m_timestep_scalar						 = 1.f;			// Factor deciding the magnitude of timesteps for each frame.

	//Visualisation
	bool					m_showDataVectors;  //TODO
	std::vector<glm::vec2>	m_translations;		//TODO
	Rect*					m_quad;				//TODO
	Line*					m_line;				//TODO

	Mouse  m_mouse;

	//Initialize the grid (Not nessecary!)
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution) :
		m_resolution(resolution),
		m_fieldWidth((int)sqrt(resolution)),
		m_WIDTH(WIDTH),
		m_HEIGHT(HEIGHT),
		m_mouse(WIDTH, HEIGHT),

		m_advection_shader(p_VERTEX_SHADER, p_advection_shader),
		m_jacobi_iteration_shader(p_VERTEX_SHADER, p_jacobi_shader),
		m_force_shader(p_VERTEX_SHADER, p_force_shader),
		m_divergence_shader(p_VERTEX_SHADER, p_divergence_shader),
		m_clear_shader(p_VERTEX_SHADER, p_clear_shader),
		m_gradient_subtraction_shader(p_VERTEX_SHADER, p_gradient_subtraction_shader),
		m_vorticity_shader(p_VERTEX_SHADER, p_vorticity_shader),
		m_bounds_shader(p_VERTEX_SHADER, p_bounds_shader),
		m_splat_shader(p_VERTEX_SHADER, p_splat_shader)
	{
		std::cout << "INITIALIZING::FLUIDFIELD" << std::endl;

		m_primary_shader		 = new Shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
		m_visualise_grid_shader	 = new Shader(p_VISUALISE_GRID_VERTEX_SHADER, p_VISUALISE_GRID_FRAGMENT_SHADER);
		m_texture				 = new Texture2D(p_TEXTURE);
		m_texture_buffer		 = new Texture2D();
		m_quad					 = new Rect();
		m_fieldQuad				 = new Rect(
									glm::vec3(0.0f), 
									glm::vec3(0.0f), 
									glm::vec3(0.0f), 
									m_texture->get());
		//&m_translations.resize(resolution);		//reserve memory
		Init();

		GLenum textureType = GL_UNSIGNED_BYTE;	//Field type
		TexFormat rgba(GL_RGBA32F, GL_RGBA);	//Quantity field
		TexFormat rg(GL_RG32F, GL_RG);			//Vector field
		TexFormat r(GL_R32F, GL_RED);			//Scalar field
		glDisable(GL_BLEND);
		// Dye
		m_dye_buffer = new DoubleFramebuffer(m_resolution, m_WIDTH, m_HEIGHT, rgba.internal, rgba.format, textureType, GL_LINEAR);
		//m_dye_buffer->readBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
		//m_dye_buffer->writeBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
		// Velocity
		m_velocity_buffer = new DoubleFramebuffer(m_resolution, m_WIDTH, m_HEIGHT, rg.internal, rg.format, textureType, GL_LINEAR);
		//Vorticity
		m_vorticity_buffer = new DoubleFramebuffer(m_resolution, m_WIDTH, m_HEIGHT, rg.internal, rg.format, textureType, GL_LINEAR);
		// Divergence
		m_divergence_buffer = new Framebuffer(m_resolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		// Curl
		m_curl_buffer = new Framebuffer(m_resolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		// Pressure
		m_pressure_buffer = new DoubleFramebuffer(m_resolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);

		m_current_buffer = m_dye_buffer->readBuffer();

		std::cout << "SUCCESS::INITIALIZATION::FLUIDFIELD" << std::endl;
	}

	~FluidField() {
		delete(m_dye_buffer);
		delete(m_velocity_buffer);
		delete(m_vorticity_buffer);
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
	//Draw a visual representation of the dimensions of a grid containing data
	//NOTE: Not sure if its working. 
	//TODO: Create shader that draws when this function is called 
	void DrawCellField(glm::vec3 origin);
	//Set mouse position and button properties
	void updateMouse(double* mouseX, double* mouseY, bool* mouse_down);
	//Move forward in time, update values
	void timeStep(float dt); 
	void swapBuffer(int i);

private:
	//Draw using specified shader together with a specified framebuffer (NULL if the purpose is to render to the screen with specified shader)
	void blit(Framebuffer* target, Shader* shader);
	void advect(float dt);
	void diffuse(float dt);
	void addForces(float dt); //TODO
	void project(float dt);

	void vorticity(float dt);
	void divergence(float dt);
	void clearBuffer(DoubleFramebuffer* target, float value);
	void clearBuffer(Framebuffer* target, float value);
	void pressure(float dt);
	void gradientSubtract(float dt);
	/// <summary>
	///Add velocity in the specified position
	/// </summary>
	/// <param name="pos">Origin</param>
	/// <param name="r">Radius of the splat</param>
	void splat(glm::vec2 pos, float r);
	//Clear everything and start from the beginning
	void reset(); //TODO
};

