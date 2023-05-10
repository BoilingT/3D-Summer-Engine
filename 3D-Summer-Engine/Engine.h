#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

//GLM
#include "glm_includes.h"

#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#include "WindowHandler.h"
#include "fileHandler.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "object_includes.h"
#include "FluidField.h"
#include "Compute.h"


class Engine
{
public:
	//Mouse Events (not included)
	static double g_lastX;
	static double g_lastY;
	static bool g_mouseDown;
	static bool g_firstMouseEnter;
	static bool g_mouse_constrain;

	//Fluid simulation
	static unsigned int g_running;

private:
	WindowHandler*		m_window;
	Camera*				m_camera;
	FluidField*			m_fluid;
	FileHandler			fileSystem;

	//Window Properties
	static const int c_WIDTH								 = 1920;
	static const int c_HEIGHT								 = 1080;
	const char* c_WINDOW_NAME								 = "Summer Engine";
	const float c_DEFAULT_CLEAR_COLOR[4]					 = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float c_CLEAR_COLOR[4]							 = { 0.28f, 0.41f, 0.61f, 1.0f };

	//Filepaths
	const char* p_DEFAULT_VERTEX_SHADER						 = "Shaders/default_vertex_shader.vert";
	const char* p_DEFAULT_FRAGMENT_SHADER					 = "Shaders/default_fragment_shader.frag";
	const char* p_CONTAINER_IMAGE							 = "Images/LearnOpenGL/container.jpg";
	const char* p_AWESOMEFACE_IMAGE							 = "Images/LearnOpenGL/awesomeface.png";
	const char* p_GENERATED_RESULTS							 = "Images/Generated Results/";
	
	//Engine variables
	float g_deltaTime										 = 0.0f;
	float g_pc_time											 = 0.0f;
	bool  g_save_result										 = false;
	float g_save_result_time								 = 20.0f;

	//Engine Properties
	float g_fps_limit										 = 0.0f;			// Monitor refreshrate: (x < 0), No limit: (x = 0)

	//Fluid Simulation Properties
	const int   c_RESOLUTION								 = 512;				// aka Gridarea
	const float c_precision									 = 1.0f / 144.0f;	// Simulator speed
	const bool  c_precision_bound							 = false;			// If the simulator should account for time lost by lag

public:

	Engine()
	{
		std::cout << "INITIALIZING::ENGINE" << std::endl;

		m_window = new WindowHandler(c_WIDTH, c_HEIGHT, c_WINDOW_NAME);
		m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		
		Init();
		std::cout << "SUCCESS::INITIALIZATION::ENGINE" << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Engine Initialized");
	}

	~Engine() {
		glfwTerminate();

		delete m_fluid;
		delete m_camera;
		delete m_window;

		m_window = NULL;
		m_camera = NULL;
		m_fluid = NULL;
		std::cout << "DESTROYED::ENGINE" << std::endl;
	}

	//Initialize the window and its components
	void Init();
	//Start the rendering sequence
	void Run();
	
	void saveImage(const char* path, GLFWwindow* window);
	void saveResults();

private:
	static void FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height);
	static void MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos);
	static void KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods);
	void IO_EVENTS(GLFWwindow* window);
};

