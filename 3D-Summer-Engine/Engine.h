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
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
	WindowHandler* _window;
	Shader* _shader;
	Camera* _camera;
	Compute* _compute_shader;

	FileHandler fileSystem;

	static const int c_WIDTH = 1920;
	static const int c_HEIGHT = 1080;
	const char* c_WINDOW_NAME = "Summer Engine";
	const float c_DEFAULT_CLEAR_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float c_CLEAR_COLOR[4] = { 0.28f, 0.41f, 0.61f, 1.0f };

	const char* p_COMPUTE_SHADER = "Shaders/compute_shader.glsl";
	const char* p_VERTEX_SHADER = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER = "Shaders/fragment_shader.frag";
	const char* p_CONTAINER_IMAGE = "Images/LearnOpenGL/container.jpg";
	const char* p_AWESOMEFACE_IMAGE = "Images/LearnOpenGL/awesomeface.png";

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	float lastX = Engine::c_WIDTH / 2;
	float lastY = Engine::c_HEIGHT / 2;
	bool firstMouseEnter = true;

	Engine() {
		std::cout << "Initializing Engine..." << std::endl;

		_window = new WindowHandler(c_WIDTH, c_HEIGHT, c_WINDOW_NAME);
		_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		Init();
		std::cout << "Engine Initialized" << std::endl;
		glfwSetWindowTitle(_window->getWindow(), "Engine Initialized");
	}

	~Engine() {
		std::cout << "Terminating..." << std::endl;
		glfwSetWindowTitle(_window->getWindow(), "Terminating...");
		glfwTerminate();
		std::cout << "Engine Stopped" << std::endl;

		delete _window;
		delete _camera;
		delete _compute_shader;
		_window = NULL;
		_camera = NULL;
		_compute_shader = NULL;
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
	void POLL_EVENTS(GLFWwindow* window);
};

