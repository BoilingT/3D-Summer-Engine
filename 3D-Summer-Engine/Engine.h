#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "WindowHandler.h"
#include "fileHandler.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Cube.h"
#include "Plane.h"
#include "FluidField.h"
#include "Compute.h"

class Engine
{
public:
	WindowHandler*		m_window;
	Shader*				m_shader;
	Camera*				m_camera;
	Compute*			m_compute_shader;

	FileHandler fileSystem;

	//Window Properties
	static const int c_WIDTH				 = 1920;
	static const int c_HEIGHT				 = 1080;
	const char*	c_WINDOW_NAME				 = "Summer Engine";
	const float	c_DEFAULT_CLEAR_COLOR[4]	 = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float	c_CLEAR_COLOR[4]			 = { 0.28f, 0.41f, 0.61f, 1.0f };

	//Filepaths
	const char* p_COMPUTE_SHADER			 = "Shaders/compute_shader.glsl";
	const char* p_VERTEX_SHADER				 = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER			 = "Shaders/fragment_shader.frag";
	const char* p_CONTAINER_IMAGE			 = "Images/LearnOpenGL/container.jpg";
	const char* p_AWESOMEFACE_IMAGE			 = "Images/LearnOpenGL/awesomeface.png";
	
	//Mouse Events (not included)
	float lastX								 = Engine::c_WIDTH / 2;
	float lastY								 = Engine::c_HEIGHT / 2;
	bool firstMouseEnter					 = true;

	//dT
	float deltaTime							 = 0.0f;
	float lastTime							 = 0.0f;

	//Fluid Simulation
	const int c_resolution					 = 128; // aka Gridarea

	Engine() {
		std::cout << "Initializing Engine..." << std::endl;

		m_window = new WindowHandler(c_WIDTH, c_HEIGHT, c_WINDOW_NAME);
		m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		Init();
		std::cout << "Engine Initialized" << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Engine Initialized");
	}

	~Engine() {
		std::cout << "Terminating..." << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Terminating...");
		glfwTerminate();
		std::cout << "Engine Stopped" << std::endl;

		delete m_window;
		delete m_camera;
		delete m_compute_shader;
		m_window = NULL;
		m_camera = NULL;
		m_compute_shader = NULL;
		std::cout << "Window has been destroyed" << std::endl;
	}

	/*
		Initialize the window and its components
	*/
	void Init();
	/*
		Start the rendering sequence
	*/
	void Run();

private:
	static void FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height);
	static void MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos);
	void IO_EVENTS(GLFWwindow* window);
};

