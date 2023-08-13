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

class FluidField
{
	struct Mouse {
		bool left_mouse_down = false;
		bool right_mouse_down = false;
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

		void updateMousearea(double _width, double _height) {
			width = _width;
			height = _height;
		}

		void update(double mouseX, double mouseY, bool left_mouse_is_down, bool right_mouse_is_down) {
			window_delta			 = glm::vec2(mouseX, mouseY) - prev_window_pos;
			prev_window_pos			 = window_pos;
			window_pos				 = glm::vec2(mouseX, mouseY);
			window_travel			 += window_travel;

			texcoord_delta			 = texcoord_pos - prev_texcoord_pos;
			prev_texcoord_pos		 = texcoord_pos;
			texcoord_pos			 = glm::vec2(mouseX / width, 1-mouseY / height);
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
		DoubleFramebuffer(float res, unsigned int width, unsigned int height, GLint internalFormat, GLenum format, GLenum type, GLint param) {
			fb1 = new Framebuffer(res, width, height, internalFormat, format, type, param);
			fb2 = new Framebuffer(res, width, height, internalFormat, format, type, param);
		}

		~DoubleFramebuffer() {
			delete(fb1);
			delete(fb2);
		}

		void setDimensions(unsigned int width, unsigned int height, float res) {
			fb1->setDimensions(width, height, res);
			fb2->setDimensions(width, height, res);
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
	const char* p_CONFIG_FILE							 = "./fluid_config.cfg";
	const char* p_COMPUTE_SHADER						 = "./Shaders/compute_shader_backup.glsl";
	const char* p_advection_shader						 = "./Shaders/compute_advection_shader.frag";
	const char* p_jacobi_shader							 = "./Shaders/compute_jacobi_shader.frag";
	const char* p_force_shader							 = "./Shaders/compute_force_shader.frag";
	const char* p_divergence_shader						 = "./Shaders/compute_divergence_shader.frag";
	const char* p_clear_shader							 = "./Shaders/compute_clear_shader.frag";
	const char* p_integrate_shader						 = "./Shaders/compute_integrate_shader.frag";
	const char* p_gradient_subtraction_shader			 = "./Shaders/compute_gradient_subtraction_shader.frag";
	const char* p_vorticity_shader						 = "./Shaders/compute_vorticity_shader.frag";
	const char* p_curl_shader							 = "./Shaders/compute_curl_shader.frag";
	const char* p_temperature_shader					 = "./Shaders/compute_temperature_shader.frag";
	const char* p_density_shader						 = "./Shaders/compute_density_shader.frag";
	const char* p_bounds_shader							 = "./Shaders/compute_bounds_shader.frag";
	const char* p_splat_shader							 = "./Shaders/compute_splat_shader.frag";

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
	struct FluidConfigKeys {
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
	DoubleFramebuffer* m_velocity_buffer;				//Contains velocities to be advected
	Framebuffer* m_divergence_buffer;					//Contains divergent velocities
	DoubleFramebuffer* m_pressure_buffer;				//Contains a pressure field

	//Application buffers
	DoubleFramebuffer* m_dye_buffer;					//Contains dye quantities to be advected
	DoubleFramebuffer* m_temperature_buffer;			//Boyancy and Convection
	DoubleFramebuffer* m_density_buffer;				//Boyancy and Convection
	Framebuffer* m_curl_buffer;							//Contains curling velocities
	//Screen rendering buffers
	Framebuffer* m_current_buffer;						//Contains the buffer that will be used when rendering

	//Shaders
	Shader m_advection_shader;							//Used for advecting quantities in the fluid dependent on the given velocity buffer
	Shader m_jacobi_iteration_shader;					//Used for Pressure and Diffusion
	Shader m_force_shader;								//TODO: Used for application of force (Velocities)
	Shader m_divergence_shader;							//Calculates change in density (Density velocities)
	Shader m_integrate_shader;							//Used for adding a value to an entire buffer
	Shader m_clear_shader;								//Used for clearing a buffer of its values
	Shader m_gradient_subtraction_shader;				//Subtract a gradient from given buffer
	Shader m_vorticity_shader;							//Adds swirly movement and details lost by numerical error.
	Shader m_curl_shader;								//Calculates how the fluid curls
	Shader m_temperature_shader;						//TODO
	Shader m_density_shader;							//TODO
	Shader m_bounds_shader;								//TODO: Control of Fluid boundaries
	Shader m_splat_shader;								//Used for application of Dye and Velocity manipulation
	//Screen rendering shaders
	Shader  m_object_shader;							//Used to render objects to the screen
	Shader* m_primary_shader;							//Used to render the fluid to the screen
	Shader* m_visualise_grid_shader;					//TODO: Used to render a visual representation of the resolution used to the screen (Is not being used)

	Texture2D*	 m_texture;
	Texture2D*	 m_texture_buffer;
	Rect*		 m_fieldQuad;
	const float	 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	float	 m_dye_scalar							 = 1.0; //This makes the dye resolution always larger than the velocity resolution, this achieves better details
	float	 m_velocity_scalar						 = 1.0f;
	float	 m_dye_color[3]							 = { 0.0f, 0.1f, 1.0f };
	float	 m_dye_brightness						 = 0.5f;
	bool	 m_dye_color_acc_dependent				 = 0;			// If color should depend on mouse acceleration
	float	 m_dye_force							 = 6000.0f;		// Force used to create velocities
	float	 m_dye_radius							 = 0.35f;		// Radius of the applicable dye and velocites
	float	 m_dye_dissipation						 = 0.2f;		// The rate at which the dye clears from the screen
	float	 m_velocity_dissipation					 = 0.1f;		// The rate at which the velocities reduces to zero
	int		 m_diffuseIterations					 = 30;			// Number of iterations used to calculate proper diffusion of the applied dye or velocities
	float	 m_viscosity							 = 0.0f;		// Drag factor of the fluid
	int		 m_pressureIterations					 = 60;			// Number of iterations used to calculate more precise pressure fields
	float	 m_pressure_dissipation					 = 0.9f;		// TODO: Explain it... What I thought it was (but it is supposedly wrong!): The rate at which the pressure field is cleared
	float	 m_vortitcity_scalar					 = 30;			// Vorticity scalar
	float	 m_timestep_scalar						 = 1.00f;		// Factor deciding the magnitude of timesteps for each frame.
	
	int		 m_splats								 = 11;
	bool	 m_advect								 = 1;
	bool	 m_diffuse								 = 1;
	bool	 m_forces								 = 0;
	bool	 m_project								 = 1;
	bool	 m_image								 = 0;

	//Experimental
	//const float  m_ambient_temperature					 = 18.0f;	// Ambient temperature in degrees celsius
	//const float  m_temperature_scalar					 = 10.0f;		// Scales the effect that the difference in temperature has on the boyant force
	//const float  m_mass									 = 3.0f;	// Smoke mass (Dye mass)
	//const float  m_density								 = 1.8f;	// Smoke density (Dye density)
	//Experimental (Is not in use)
	const float  m_ambient_temperature					 = -7.0f;		// Ambient temperature in degrees celsius
	const float  m_temperature_scalar					 = 25.0f;		// Scales the effect that the difference in temperature has on the boyant force
	const float  m_mass									 = 10.0f;		// Smoke mass (Dye mass) //Downforce
	const float  m_density								 = 0.6f;		// Smoke density (Dye density) //Downforce

	//Visualisation
	Rect					rectangle;
	Line					line;

	Mouse  m_mouse;

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution) :
		m_resolution(resolution),
		m_fieldWidth((int)sqrt(resolution)),
		m_WIDTH(WIDTH),
		m_HEIGHT(HEIGHT),
		m_mouse(WIDTH, HEIGHT),
		rectangle(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f)),
		line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f),

		fluid_config(p_CONFIG_FILE),

		m_object_shader(p_OBJECT_VERTEX_SHADER, p_OBJECT_FRAGMENT_SHADER),
		m_advection_shader(p_VERTEX_SHADER, p_advection_shader),
		m_jacobi_iteration_shader(p_VERTEX_SHADER, p_jacobi_shader),
		m_force_shader(p_VERTEX_SHADER, p_force_shader),
		m_divergence_shader(p_VERTEX_SHADER, p_divergence_shader),
		m_clear_shader(p_VERTEX_SHADER, p_clear_shader),
		m_integrate_shader(p_VERTEX_SHADER, p_integrate_shader),
		m_gradient_subtraction_shader(p_VERTEX_SHADER, p_gradient_subtraction_shader),
		m_vorticity_shader(p_VERTEX_SHADER, p_vorticity_shader),
		m_curl_shader(p_VERTEX_SHADER, p_curl_shader),
		m_temperature_shader(p_VERTEX_SHADER, p_temperature_shader),
		m_density_shader(p_VERTEX_SHADER, p_density_shader),
		m_bounds_shader(p_VERTEX_SHADER, p_bounds_shader),
		m_splat_shader(p_VERTEX_SHADER, p_splat_shader)
	{
		std::cout << "APPLYING::CONFIGURATIONS" << std::endl;

		if (fluid_config.fileExists()) applyConfiguration(fluid_config);

		std::cout << "INITIALIZING::FLUIDFIELD" << std::endl;
		
		m_primary_shader		 = new Shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
		m_visualise_grid_shader	 = new Shader(p_VISUALISE_GRID_VERTEX_SHADER, p_VISUALISE_GRID_FRAGMENT_SHADER);
		m_texture				 = new Texture2D(p_TEXTURE);
		m_texture_buffer		 = new Texture2D();

		//This is the rectangle that is used for displaying the simulation
		//The simulation is simply a texture drawn on this rectangle
		m_fieldQuad				 = new Rect(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), m_texture->get());
		
		m_mouse.updateMousearea(m_WIDTH, m_HEIGHT);

		GLenum textureType = GL_UNSIGNED_BYTE;	//Field type
		TexFormat rgba(GL_RGBA32F, GL_RGBA);	//Quantity field
		TexFormat rg(GL_RG32F, GL_RG);			//Vector field
		TexFormat r(GL_R32F, GL_RED);			//Scalar field
		glDisable(GL_BLEND);

		float velocityResolution = m_resolution * m_velocity_scalar, dyeResolution = m_resolution * m_dye_scalar;

		//Buffers that store the calculated results
		m_dye_buffer = new DoubleFramebuffer(dyeResolution, m_WIDTH, m_HEIGHT, rgba.internal, rgba.format, textureType, GL_LINEAR);
		//m_dye_buffer->readBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
		//m_dye_buffer->writeBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
		m_velocity_buffer = new DoubleFramebuffer(velocityResolution, m_WIDTH, m_HEIGHT, rg.internal, rg.format, textureType, GL_LINEAR);
		m_curl_buffer = new Framebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		m_divergence_buffer = new Framebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		m_pressure_buffer = new DoubleFramebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		
		//Experimental
		// Boyancy and Convection
		m_temperature_buffer = new DoubleFramebuffer(dyeResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
		m_temperature_buffer->readBuffer()->setTextureSource(p_TEMPERATURE_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_NEAREST);
		m_temperature_buffer->writeBuffer()->setTextureSource(p_TEMPERATURE_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_NEAREST);
		
		// Smoke and Clouds
		m_density_buffer = new DoubleFramebuffer(dyeResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);

		m_current_buffer = m_dye_buffer->readBuffer();

		std::cout << "SUCCESS::INITIALIZATION::FLUIDFIELD" << std::endl;
	}

	void updateViewport(float width, float height) {
		float velocityResolution = m_resolution * m_velocity_scalar, dyeResolution = m_resolution * m_dye_scalar;
		m_mouse.updateMousearea(width, height);
		m_dye_buffer->setDimensions(m_mouse.width, m_mouse.height, dyeResolution);
		m_velocity_buffer->setDimensions(m_mouse.width, m_mouse.height, velocityResolution);
		m_curl_buffer->setDimensions(m_mouse.width, m_mouse.height, velocityResolution);
		m_divergence_buffer->setDimensions(m_mouse.width, m_mouse.height, velocityResolution);
		m_pressure_buffer->setDimensions(m_mouse.width, m_mouse.height, velocityResolution);
	}

	~FluidField() {
		//There is probably a better way to do this
		delete(m_dye_buffer);
		delete(m_velocity_buffer);
		delete(m_temperature_buffer);
		delete(m_density_buffer);
		delete(m_curl_buffer);
		delete(m_divergence_buffer);
		delete(m_pressure_buffer);

		delete(m_primary_shader);
		delete(m_visualise_grid_shader);
		delete(m_fieldQuad);
		std::cout << "DESTROYED::FLUIDFIELD" << std::endl;
	}

	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Set mouse position and button properties
	void updateMouse(double* mouseX, double* mouseY, bool* left_mouse_down, bool* right_mouse_down);
	//Move forward in time, update values
	void timeStep(float dt); 
	void setCurrentBuffer(Framebuffer* buffer);
	void swapBuffer(int i);

	//Clear everything and start from the beginning
	void reset(); //TODO
	int applyConfiguration(Config& configurationFile);
	void updateConfiguration();

private:
	//Draw using specified shader together with a specified framebuffer (NULL if the purpose is to render to the screen with specified shader)
	void blit(Framebuffer* target, Shader* shader);
	void border(Framebuffer* target);
	void bufferIntegrate(DoubleFramebuffer* target, glm::vec4 value);
	void advect(float dt);
	void diffuse(float dt);
	void addForces(float dt);
	void project(float dt);

	void boundaryContainer(bool l, bool r, bool t, bool b, Framebuffer* target, Shader& shader);
	void boundary(float dt, float scale, float offset, DoubleFramebuffer* target);
	void boundaries(float dt);
	void vorticity(float dt);
	void curl(float dt);
	void temperature(float dt);
	void density(float dt);
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
	void splat(glm::vec2 pos, float r, bool dye, bool velocity);
	void splat(glm::vec2 pos, float r, unsigned int amount, bool dye, bool velocity);
};

