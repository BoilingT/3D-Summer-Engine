#pragma once
#include <iostream>
#include <string>

#include <thread>
#include <chrono>

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
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

/*
	Graphics Pipeline:
		Vertex data -> Vertex Shader -> Shape Assembly (Opt) -> Geometry Shader -> Rasterization (Opt) -> Fragment Shader -> Tests and Blending (Opt)
		Essentials: Vertex data -> Vertex Shader -> Geometry Shader -> Fragent Shader

		Vertex Shader:
			The vertex shader takes a single vertex as data input. Its main purpose is to transform 3D coordinates into different 3D coordinates.
			It also allows for basic processing on the vertex attributes.

		Shape Assembly:
			The assembly stage uses the data from all vertices to create a primitve form and "assembles all the point(s) in the primitive shape given",
			as for an example a triangle.

		Geometry Shader:
			The data collection from the assembly stage is transferred onto the geometry shader. Its job is to take a collection of vertices
			that form a primitive form. It has the ability to use the primitive to generate other shapes by making new vertices to form new or different
			primitives.

		Rasterization:
			The data from the Geometry Shader is passed on to the Rasterization stage where it maps the resulting primitives
			to the corresponding pixels on the screen. This results in fragments for the fragment shader to use. Before the fragment shaders run
			fragments that are outside the screen are clipped and discarded which improves the performance.

		Fragment Shader:
			A Fragments Shader's main purpose is to calculate the final color of a pixel. During this stage is usally when all
			the advanced OpenGL effects occur. The Fragment Shader usally contains data bout the 3D scene that it can use to calculate the final pixel color.

		Tests and Blending
			After all the color values have been determined the final object will then go through the "Alpha test" and "Blending stage". This stage checks the
			depth values of the fragment to determine if a fragment is in front or behind another object and should thus be discarded accordingly.
	*/

class Engine
{
private:


public:
	WindowHandler* _window;
	Shader* _shader;
	Camera* _camera;

	FileHandler fileSystem;

	static const int c_WIDTH = 1920;
	static const int c_HEIGHT = 1080;
	const char* c_WINDOW_NAME = "Summer Engine";
	const float c_DEFAULT_CLEAR_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float c_CLEAR_COLOR[4] = { 0.28f, 0.41f, 0.61f, 1.0f };

	const char* p_VERTEX_SHADER = "Shaders/vertex_shader.vert";
	const char* p_FRAGMENT_SHADER = "Shaders/fragment_shader.frag";
	const char* p_CONTAINER_IMAGE = "Images/LearnOpenGL/container.jpg";
	const char* p_AWESOMEFACE_IMAGE = "Images/LearnOpenGL/awesomeface.png";

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
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
		_window = NULL;
		_camera = NULL;
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
	static void FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height);
	static void MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos);

private:
	void POLL_EVENTS(GLFWwindow* window);
};

