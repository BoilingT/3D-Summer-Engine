#pragma once
#include <iostream>
#include "config.h"

#include "object_includes.h"
#include "Line.h"

#include "Shader.h"
#include "Framebuffer.h"
#include "Texture2D.h"
#include "Compute.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

class FluidSimulation
{
	struct Mouse
	{
		bool left_mouse_down = false;
		bool right_mouse_down = false;
		unsigned int height, width;

		glm::vec2 texcoord_travel	 = glm::vec2(0.0f);
		glm::vec2 texcoord_pos		 = glm::vec2(0.0f);
		glm::vec2 prev_texcoord_pos	 = glm::vec2(0.0f);
		glm::vec2 texcoord_delta	 = glm::vec2(0.0f);

		glm::vec2 window_travel		 = glm::vec2(0.0f);
		glm::vec2 window_pos		 = glm::vec2(0.0f);
		glm::vec2 prev_window_pos	 = glm::vec2(0.0f);
		glm::vec2 window_delta		 = glm::vec2(0.0f);

		Mouse(unsigned int _width, unsigned int _height)
		{
			width = _width;
			height = _height;
		}

		void updateMousearea(unsigned int _width, unsigned int _height)
		{
			width = _width;
			height = _height;
		}

		void update(double mouseX, double mouseY, bool left_mouse_is_down, bool right_mouse_is_down)
		{
			window_delta			 = glm::vec2(mouseX, mouseY) - prev_window_pos;
			prev_window_pos			 = window_pos;
			window_pos				 = glm::vec2(mouseX, mouseY);
			window_travel			 += window_travel;

			texcoord_delta			 = texcoord_pos - prev_texcoord_pos;
			prev_texcoord_pos		 = texcoord_pos;
			texcoord_pos			 = glm::vec2(mouseX / width, 1 - mouseY / height);
			texcoord_travel			 += texcoord_travel;
			left_mouse_down			 = left_mouse_is_down;
			right_mouse_down		 = right_mouse_is_down;

			if (!left_mouse_is_down && !right_mouse_is_down)
			{
				texcoord_travel		 = glm::vec2(0.0f);
				window_travel		 = glm::vec2(0.0f);
			}
		}
	};

	struct DoubleFramebuffer
	{
	private:
		Framebuffer* fb1;
		Framebuffer* fb2;
	public:
		DoubleFramebuffer(unsigned int res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param)
		{
			fb1 = new Framebuffer(res, width, height, internalFormat, format, type, param);
			fb2 = new Framebuffer(res, width, height, internalFormat, format, type, param);
		}

		~DoubleFramebuffer()
		{
			delete(fb1);
			delete(fb2);
		}

		void setDimensions(unsigned int width, unsigned int height, unsigned int res)
		{
			fb1->setDimensions(width, height, res);
			fb2->setDimensions(width, height, res);
		}

		//Get the currently bound buffer which has the purpose of being read from.
		Framebuffer* readBuffer()
		{
			return fb1;
		}

		//Set the currently bound buffer for reading to another specified buffer
		void readBuffer(Framebuffer* buffer)
		{
			fb1 = buffer;
		}

		//Get the currently bound buffer which has the purpose of being written to.
		Framebuffer* writeBuffer()
		{
			return fb2;
		}

		//Set the currently bound writing for reading to another specified buffer
		void writeBuffer(Framebuffer* buffer)
		{
			fb2 = buffer;
		}

		//Swap buffers so that the write buffer becomes the readbuffer or vice versa
		void swap()
		{
			Framebuffer* temp = fb1;
			fb1 = fb2;
			fb2 = temp;
		}
	};

	struct TexFormat
	{
		const GLint internal; //Type of data to be stored
		const GLenum format; //Source data type
		TexFormat(GLint internalFormat, GLenum format) : internal(internalFormat), format(format) {}
	};


private:
	//Simulation Shaders 
	const char* p_CONFIG_FILE							 = "./fluid_config.cfg";
	const char* p_COMPUTE_SHADER						 = "./Shaders/FluidSimulation/shader_backup.glsl";
	const char* p_advection_shader						 = "./Shaders/FluidSimulation/advection_shader.frag";
	const char* p_jacobi_shader							 = "./Shaders/FluidSimulation/jacobi_shader.frag";
	const char* p_force_shader							 = "./Shaders/FluidSimulation/force_shader.frag";
	const char* p_divergence_shader						 = "./Shaders/FluidSimulation/divergence_shader.frag";
	const char* p_clear_shader							 = "./Shaders/FluidSimulation/clear_shader.frag";
	const char* p_integrate_shader						 = "./Shaders/FluidSimulation/integrate_shader.frag";
	const char* p_gradient_subtraction_shader			 = "./Shaders/FluidSimulation/gradient_subtraction_shader.frag";
	const char* p_vorticity_shader						 = "./Shaders/FluidSimulation/vorticity_shader.frag";
	const char* p_curl_shader							 = "./Shaders/FluidSimulation/curl_shader.frag";
	const char* p_temperature_shader					 = "./Shaders/FluidSimulation/temperature_shader.frag";
	const char* p_density_shader						 = "./Shaders/FluidSimulation/density_shader.frag";
	const char* p_bounds_shader							 = "./Shaders/FluidSimulation/bounds_shader.frag";
	const char* p_splat_shader							 = "./Shaders/FluidSimulation/splat_shader.frag";
	const char* p_apply_shader							 = "./Shaders/FluidSimulation/apply_shader.frag";

	//Engine Shaders
	const char* p_VISUALISE_GRID_VERTEX_SHADER			 = "./Shaders/visualise_grid_vertex_shader.vert";
	const char* p_VISUALISE_GRID_FRAGMENT_SHADER		 = "./Shaders/visualise_grid_fragment_shader.frag";
	const char* p_VERTEX_SHADER							 = "./Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER						 = "./Shaders/fragment_shader.frag";
	const char* p_OBJECT_VERTEX_SHADER					 = "./Shaders/object_vertex_shader.vert";
	const char* p_OBJECT_FRAGMENT_SHADER				 = "./Shaders/object_fragment_shader.frag";
	const char* p_TEXTURE								 = "./Images/Generated Images/notGenerated.jpg";
	const char* p_TEMPERATURE_TEXTURE					 = "./Images/Generated Images/temp.png";

	//Config files
	Config fluid_config;

	//Keys
	struct FluidConfigKeys
	{
		const std::string dye_scalar = "dye_resolution_scalar";
		const std::string velocity_scalar = "velocity_resolution_scalar";
		const std::string dye_color = "dye_color";
		const std::string dye_brightness = "dye_brightness";
		const std::string dye_color_acc_dependent = "dye_color_acc_dependent";
		const std::string dye_force = "dye_force";
		const std::string dye_radius = "dye_radius";
		const std::string dye_dissipation = "dye_dissipation";
		const std::string velocity_dissipation = "velocity_dissipation";
		const std::string diffuseIterations = "diffuseIterations";
		const std::string viscosity = "viscosity";
		const std::string pressureIterations = "pressureIterations";
		const std::string pressure_dissipation = "pressure_dissipation";
		const std::string vortitcity_scalar = "vortitcity_scalar";
		const std::string timestep_scalar = "timestep_scalar";
		const std::string splats = "splats";
		const std::string advect = "advect";
		const std::string diffuse = "diffuse";
		const std::string forces = "forces";
		const std::string project = "project";
		const std::string image = "image";
	} FLUID;

	//Framebuffers
	DoubleFramebuffer* m_velocity_buffer; //Contains velocities to be advected
	DoubleFramebuffer* m_pressure_buffer; //Contains a pressure field
	DoubleFramebuffer* m_dye_buffer;      //Contains dye quantities to be advected
	Framebuffer* m_divergence_buffer;     //Contains divergent velocities
	Framebuffer* m_curl_buffer;           //Contains curling velocities
	Framebuffer* m_render_buffer;         //Contains the buffer that will be used when rendering

	//Shaders
	Shader m_advection_shader;            //Used for advecting quantities in the fluid dependent on the given velocity buffer
	Shader m_jacobi_iteration_shader;     //Used for Pressure and Diffusion
	Shader m_force_shader;                //Alternative shader for splat shader (Not ready)
	Shader m_divergence_shader;           //Calculates change in velocities
	Shader m_integrate_shader;            //Used for adding a value to an entire buffer
	Shader m_clear_shader;                //Used for clearing a buffer of its values
	Shader m_gradient_subtraction_shader; //Subtract a gradient from given buffer
	Shader m_vorticity_shader;            //Adds swirly movement and details lost by numerical error.
	Shader m_curl_shader;                 //Calculates how the fluid curls
	Shader m_bounds_shader;               //TODO: Control of Fluid boundaries
	Shader m_splat_shader;                //Used for application of Dye and Velocity manipulation
	Shader m_apply_shader;                //Used for application of value
	//Screen rendering shaders
	Shader m_object_shader;               //Used to render objects to the screen
	Shader m_primary_shader;              //Used to render the fluid to the screen

	Texture2D* m_texture;
	Rect* m_fieldQuad;
	Mouse m_mouse;
	Rect rectangle;
	Line line;

	const unsigned int m_WIDTH, m_HEIGHT, m_resolution;

	float m_velocity_resolution_scalar			 = 11.0f;
	float m_dye_resolution_scalar				 = 2.0f;
	bool  m_splat_color_acc_dependent			 = false;
	float m_splat_brightness					 = 1.0f;
	float m_splat_color[3]						 = { 0.0f, 0.1f, 1.0f };
	float m_splat_force							 = 6000.0f;
	float m_splat_radius						 = 0.25f;
	float m_dye_dissipation						 = 0.3f;		// The rate at which the dye clears from the screen
	float m_velocity_dissipation				 = 0.2f;		// The rate at which the velocities reduces to zero
	float m_viscosity							 = 0.0f;		// Internal friction of the fluid
	unsigned int m_diffuseIterations			 = 20;			// Number of iterations used to calculate proper diffusion of the applied dye or velocities
	unsigned int m_pressureIterations			 = 60;			// Number of iterations used to calculate more precise pressure fields
	float m_pressure_dissipation				 = 0.8f;		// How fast the pressure field dissipates
	float m_vortitcity_scalar					 = 30.0f;		// Scale the magnitude of force that will be used when applying curling velocities
	float m_timestep_scalar						 = 1.0f;		// Unit of time which the simulation will use when advancing forward.

	unsigned int m_splats						 = 11;
	bool m_advect								 = 1;
	bool m_diffuse								 = 1;
	bool m_forces								 = 0;
	bool m_project								 = 1;
	bool m_image								 = 0;

public:
	FluidSimulation(const unsigned int WIDTH, const unsigned int HEIGHT, const unsigned int resolution);

	~FluidSimulation()
	{
		//There is probably a better way to do this
		delete(m_dye_buffer);
		delete(m_velocity_buffer);
		delete(m_curl_buffer);
		delete(m_divergence_buffer);
		delete(m_pressure_buffer);

		delete(m_fieldQuad);
		std::cout << "DESTROYED::FLUIDFIELD" << std::endl;
	}

	void resizeViewport(unsigned int width, unsigned int height);

	void Draw(glm::vec3 origin);
	void updateMouse(double* mouseX, double* mouseY, bool* left_mouse_down, bool* right_mouse_down);
	void timeStep(float dt);
	void setDisplayBuffer(Framebuffer* buffer);
	void displayTexture(int i);
	void clearSimulationBuffers();

	int applyConfiguration(Config& configurationFile);
	void updateConfiguration();

private:
	//Draw using specified shader together with a specified framebuffer (NULL if the purpose is to render to the screen with specified shader)
	void blit(Framebuffer* target, Shader* shader);
	void bufferIntegrate(DoubleFramebuffer* target, glm::vec4 value);
	void displayFluidMotion(Framebuffer* target, glm::vec3 color);

	void splat();
	void advect(float dt);
	void diffuse(float dt);
	void addForces(float dt);
	void project(float dt);

	void vorticity(float dt);
	void curl(float dt);
	void divergence(float dt);
	void clearBuffer(DoubleFramebuffer* target, float value);
	void clearBuffer(Framebuffer* target, float value);
	void pressure(float dt);
	void gradientSubtract(float dt);

	void splat(glm::vec2 pos, float r, bool dye, bool velocity);
	void splat(glm::vec2 pos, float r, unsigned int amount, bool dye, bool velocity);
};

