#include <iostream>
#include <string>
#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#include "WindowHandler.h"
#include "fileHandler.h"

const int WIDTH = 1000;
const int HEIGHT = 700;
const char* WINDOW_NAME = "Summer Engine";

const char* VERTEX_SHADER_PATH = "Shaders/vertex_shader.vert";
const char* FRAGMENT_SHADER_PATH = "Shaders/fragment_shader.frag";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main() {

	std::cout << "Engine Starting..." << std::endl;
	
	WindowHandler windowHandler(WIDTH, HEIGHT, WINDOW_NAME);
	FileHandler fileHandler;
	
	//Create window
	int result = windowHandler.init();
	if (result == -1) //If there was an error when creating the window exit the application
	{
		return -1;
	}

	//GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR :: Unable to initialize GLAD";
		return -1;
	}

	//Set the viewport size
	glViewport(0, 0, windowHandler.getWidth(), windowHandler.getHeight());
	//Resize the viewport when the window size is changed
	glfwSetFramebufferSizeCallback(windowHandler.getWindow(), framebuffer_size_callback);

	//Triangle
	float triangleVertices[] = {
	-0.5f, -0.5f, 0.0f,	//Bottom Left
	 0.5f, -0.5f, 0.0f, //Bottom Right
	 0.0f,  0.5f, 0.0f	//Middle Top
	};

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

	/*Vertex Buffer Object(VBO)
		
	*/

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//Send the vertex data into the VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	//Vertex Shader
	//Read shader file content
	std::string vertexShaderContent = "";
	fileHandler.readFile("Shaders/vertex_shader.vert", vertexShaderContent);
	std::cout << "Vertex Shader Content: \n" << vertexShaderContent << "\n !!!End of file!!!" << std::endl;

	const char* vertexShaderSource = vertexShaderContent.c_str();;

	//Create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char compileInfo[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << compileInfo << std::endl;
	}

	//Fragment Shader
	std::string fragmentShaderContent = "";
	fileHandler.readFile(FRAGMENT_SHADER_PATH, fragmentShaderContent);
	std::cout << "Fragment Shader Content: \n" << fragmentShaderContent << "\n !!!End of file!!!" << std::endl;

	const char* fragmentShaderSource = fragmentShaderContent.c_str();

	//Create fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	success = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << compileInfo << std::endl;
	}

	//Shader Program

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	success = 0;
	glad_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(compileInfo), NULL, compileInfo);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << compileInfo << std::endl;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	std::cout << "Engine Started" << std::endl;
	
	//Draw
	while (!glfwWindowShouldClose(windowHandler.getWindow())) 
	{
		processInput(windowHandler.getWindow());

		glClearColor(0.58f, 0.71f, 0.91, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		/*Double buffer
			When rendering, the front buffer contains the final output of an image and is rendered to the screen.
			While it is being drawn to the screen a back buffer is being drawn behind the scenes in order to reduce flickering issues.
		*/
		//Swap color buffer in order to render new images
		glfwSwapBuffers(windowHandler.getWindow());
		//Check if any events have been triggered
		glfwPollEvents();
	}

	glfwTerminate();
	std::cout << "Engine Stopped" << std::endl;
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}
}