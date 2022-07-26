
#include <iostream>
#include <string>

#include <thread>
#include <chrono>

#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
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
//#include "Engine.h"


const int WIDTH = 1920;
const int HEIGHT = 1080;
const char* WINDOW_NAME = "Summer Engine";
const float DEFAULT_CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const float CLEAR_COLOR[4] = {0.28f, 0.41f, 0.61f, 1.0f};

const char* VERTEX_SHADER_PATH = "Shaders/vertex_shader.vert";
const char* FRAGMENT_SHADER_PATH = "Shaders/fragment_shader.frag";
const char* CONTAINER_IMAGE_PATH = "Images/LearnOpenGL/container.jpg";
const char* AWESOMEFACE_IMAGE_PATH = "Images/LearnOpenGL/awesomeface.png";

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//Previous mouse position during the last frame
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;
bool firstMouseEnter = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void processInput(GLFWwindow* window);
int main() {
	std::cout << "Opening Window..." << std::endl;
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

	glEnable(GL_DEPTH_TEST);

	//Set the viewport size
	glViewport(0, 0, windowHandler.getWidth(), windowHandler.getHeight());
	//Resize the viewport when the window size is changed
	glfwSetFramebufferSizeCallback(windowHandler.getWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(windowHandler.getWindow(), mouse_callback);
	glfwSetInputMode(windowHandler.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	std::cout << "Setting up Shader Program..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Setting up Shader Program...");

	Shader shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	shader.use();

	//Rectangle
	float rectVertices[] = {
		//Positions			//Colors			//Texture Coordinates
		0.5f,  0.5f, 0.0f,	5.0f, 0.0f, 0.0f,	1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,	0.0f, 5.0f, 0.0f,	1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 5.0f,	0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,	5.0f, 0.0f, 0.0f,	0.0f, 1.0f // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	
	//Triangle
	float triangleVertices[]{
		-0.5f, -0.5f, 0.0f,		5.0f, 0.0f, 0.0f,	0.0f, 0.0f,  //Bottom Left
		0.5f, -0.5f, 0.0f,		0.0f, 5.0f, 0.0f,	1.0f, 0.0f,  //Bottom Right
		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 5.0f,	0.5f, 1.0f  //Top
	};

	float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	/*Vertex Buffer Object (VBO)

	*/

	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);
	//Send the Vertex data into the VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	//Send the Indices data into the EBO buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Linking Vertex Attributes

	//Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Color Attribute
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/

	//Texture Coordinate Attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Textures
	/*
	std::cout << "Setting up textures..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Setting up textures...");

	//Generate Texture
	unsigned int textures[2];
	glGenTextures(2, textures);
	//First texture
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	//Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Texture Filtering and Mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loading and Creating Textures
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(CONTAINER_IMAGE_PATH, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//Generate Mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::TEXTURE::LOAD_FAILURE" << std::endl;
	}
	stbi_image_free(data);
	shader.setInt("texture1", 0);

	//Second texture
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	//Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Texture Filtering and Mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(AWESOMEFACE_IMAGE_PATH, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//Generate Mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::TEXTURE::LOAD_FAILURE" << std::endl;
	}
	stbi_image_free(data);
	shader.setInt("texture2", 1);
	*/

	//Draw
	std::cout << "Setting up objects to be drawn..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Setting up objects to be drawn...");

	int frames = 0;
	float time = 0;
	int fps = 0;
	float sleepTime = 0;
	Cube cube(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
	Plane plane(glm::vec3(0.0f), glm::vec3(3.0f), glm::vec3(0.0f));

	const int quadGridSize = 256;
	const int gridWidth = sqrt(quadGridSize);
	//std::vector<Plane> Quads;
	//Plane Quads[quadGridSize];

	glm::vec3 o = glm::vec3(-WIDTH / 2.f, HEIGHT / 2.f, 0);


	/*for (unsigned int r = 0; r < gridWidth; r++)
	{
		for (unsigned int c = 0; c < gridWidth; c++)
		{
			Quads.push_back(Plane());
			Plane* q = &Quads[r * gridWidth + c];
			q->transform.dim = glm::vec3(WIDTH / (float) gridWidth, HEIGHT / (float) gridWidth, 0.0f);
			q->transform.pos.x = o.x + q->transform.dim.x/2 + q->transform.dim.x * c;
			q->transform.pos.y = o.y - q->transform.dim.y / 2.f - q->transform.dim.y * r;
		}
	}*/

	glm::vec3 cubePositions[] = {
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	std::cout << "Started rendering loop..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Started rendering loop...");
	while (!glfwWindowShouldClose(windowHandler.getWindow())) 
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		time += deltaTime;
		frames++;

		if (frames >= 2)
		{
			//fps = f/s => fps = 1/dt
			//144 = 1/s => s = 1/144
			fps = frames / (time);
			//std::cout << frames << " / " << time << " = " << fps << " sleep: " << sleepTime << std::endl;
			frames = 0;
			time = 0;
			std::string title = "FPS: " + std::to_string(fps);
			glfwSetWindowTitle(windowHandler.getWindow(), title.c_str());
		}
		//sleepTime = 1000 / 60 - deltaTime;

		processInput(windowHandler.getWindow());

		//Render
		glClearColor(DEFAULT_CLEAR_COLOR[0], DEFAULT_CLEAR_COLOR[1], DEFAULT_CLEAR_COLOR[2], DEFAULT_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		*/
		//Uniform variables
		shader.use();

		//Transform Matrices
		/*
			Local Space -> (Model Matrix) -> World Space -> (View Matrix) -> View Space -> (Projection Matrix) -> Clip Space -> (Viewport Transform) -> Screen Space
			Clip Space = Projection Matrix * View Matrix * Model Matrix * Local Vector
			Projection: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
		*/

		glm::mat4 modelM			= glm::mat4(1.0f);
		glm::mat4 viewM				= glm::mat4(1.0f);
		glm::mat4 projectionM		= glm::mat4(1.0f);

		//viewM = camera.lookAt(camera.forward());
		
		//viewM = glm::lookAt(camera.getPos(), camera.getPos() + camera.forward(), camera.up());
		shader.setMat4f("view", viewM);

		//projectionM = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
		projectionM = glm::ortho(-(float)WIDTH/2, (float)WIDTH/2, -(float)HEIGHT/2, (float)HEIGHT/2, -1000.0f, 1000.0f);
		shader.setMat4f("projection", projectionM);

		glBindVertexArray(VAO);
		float timeValue = glfwGetTime();
		float val = sin(timeValue/2);
		/*for (unsigned int i = 0; i < sizeof(cubePositions)/sizeof(glm::vec3); i++)
		{
			cube.transform.pos = cubePositions[i];
			if (i == 0) {
				cube.transform.dim = glm::vec3(WIDTH, HEIGHT, 50.0f);
			}
			else
			{
				cube.transform.dim = glm::vec3(val, val, val);
			}
			cube.Draw(shader);
		}
		cube.Draw(shader);*/
		
		//plane.transform.dim = glm::vec3(WIDTH/(resolution/2.f), HEIGHT/(resolution/2.f), 0.0f);

		for (unsigned int r = 0; r < gridWidth; r++)
		{
			for (unsigned int c = 0; c < gridWidth; c++)
			{
				int vLocation = glGetUniformLocation(shader.getID(), "color");
				glUniform4f(vLocation, ((c + 1.f) / gridWidth), (r + 1.f) / gridWidth, 0.0f, 1.0f);
				//Plane q = Quads[r * gridWidth + c];
				Plane* q = &plane;
				q->transform.dim = glm::vec3(WIDTH / (float)gridWidth, HEIGHT / (float)gridWidth, 0.0f);
				q->transform.pos.x = o.x + q->transform.dim.x / 2 + q->transform.dim.x * c;
				q->transform.pos.y = o.y - q->transform.dim.y / 2.f - q->transform.dim.y * r;
				q->Draw(shader);
			}
		}

		int vertexColorLocation = glGetUniformLocation(shader.getID(), "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, val, timeValue, 1.0f);

		glBindVertexArray(0);
		/*Double buffer
			When rendering, the front buffer contains the final output of an image and is rendered to the screen.
			While it is being drawn to the screen a back buffer is being drawn behind the scenes in order to reduce flickering issues.
		*/
		//Swap color buffer in order to render new images
		glfwSwapBuffers(windowHandler.getWindow());
		//Check if any events have been triggered
		glfwPollEvents();
		lastFrame = currentFrame;

		if (sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((long)(sleepTime)));
		}
	}

	std::cout << "Cleaning up buffers..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Cleaning up buffers...");

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader.getID());

	std::cout << "Terminating..." << std::endl;
	glfwSetWindowTitle(windowHandler.getWindow(), "Terminating...");
	glfwTerminate();
	std::cout << "Engine Stopped" << std::endl;
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouseEnter)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouseEnter = false;
	}
	float xTravel, yTravel;
	xTravel = xPos - lastX;
	yTravel = yPos - lastY;
	lastX = xPos;
	lastY = yPos;

	const float sensitivity = camera.sensitivity/100.f;
	camera.processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 144);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	float cameraSpeed = 2.5f;
	
	const float cameraSensitivity = camera.sensitivity * deltaTime;

	//Forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//std::cout << "Forward: { " << camera.forward().x << ", " << camera.forward().y << ", " << camera.forward().z << " }" << std::endl;
		camera.processKeyboardInput(Camera_Movement::FORWARD, deltaTime);
	}
	//Backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboardInput(Camera_Movement::BACKWARD, deltaTime);
	}
	//Left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboardInput(Camera_Movement::LEFT, deltaTime);
	}
	//Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboardInput(Camera_Movement::RIGHT, deltaTime);
	}
	//Up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.processKeyboardInput(Camera_Movement::UP, deltaTime);
	}
	//Down
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		camera.processKeyboardInput(Camera_Movement::DOWN, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.rotate(glm::vec3(1.0f, 0.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.rotate(glm::vec3(1.0f, 0.0f, 0.0f) * -cameraSensitivity);
	}
}