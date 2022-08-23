#include <iostream>
#include <string>
#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "WindowHandler.h"
#include "fileHandler.h"
#include "Shader.h"
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int WIDTH = 1000;
const int HEIGHT = 700;
const char* WINDOW_NAME = "Summer Engine";

const char* VERTEX_SHADER_PATH = "Shaders/vertex_shader.vert";
const char* FRAGMENT_SHADER_PATH = "Shaders/fragment_shader.frag";
const char* CONTAINER_IMAGE_PATH = "Images/LearnOpenGL/container.jpg";
const char* AWESOMEFACE_IMAGE_PATH = "Images/LearnOpenGL/awesomeface.png";

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

	//Vertex Shader
	Shader shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	shader.use();

	//Rectangle
	float rectVertices[] = {
		//Positions			//Colors			//Texture Coordinates
		0.8f,  0.5f, 0.0f,	5.0f, 0.0f, 0.0f,	1.0f, 1.0f, // top right
		0.8f, -0.5f, 0.0f,	0.0f, 5.0f, 0.0f,	1.0f, 0.0f, // bottom right
		-0.8f, -0.5f, 0.0f,	0.0f, 0.0f, 5.0f,	0.0f, 0.0f, // bottom left
		-0.8f,  0.5f, 0.0f,	5.0f, 0.0f, 0.0f,	0.0f, 1.0f // top left 
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

	/*Vertex Buffer Object (VBO)

	*/

	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);
	//Send the Vertex data into the VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
	//Send the Indices data into the EBO buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Linking Vertex Attributes

	//Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Color Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Texture Coordinate Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	/*Vertex Array Object (VAO)
		
	*/

	//Textures
	
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


	std::cout << "Engine Started" << std::endl;
	glUniform4f(glGetUniformLocation(shader.getID(), "posOffset"), 0.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 trans = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "transform"), 1, GL_FALSE, glm::value_ptr(trans));

	//Draw
	while (!glfwWindowShouldClose(windowHandler.getWindow())) 
	{
		processInput(windowHandler.getWindow());

		//Render
		glClearColor(0.28f, 0.41f, 0.61f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Uniform variables
		shader.use();
		float timeValue = glfwGetTime();
		float val = sin(timeValue);
		int vertexColorLocation = glGetUniformLocation(shader.getID(), "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, val, timeValue, 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glBindVertexArray(VAO);
		/*unsigned int verticesCount = sizeof(triangleVertices) / sizeof(float) / 3;
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);*/
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		/*Double buffer
			When rendering, the front buffer contains the final output of an image and is rendered to the screen.
			While it is being drawn to the screen a back buffer is being drawn behind the scenes in order to reduce flickering issues.
		*/
		//Swap color buffer in order to render new images
		glfwSwapBuffers(windowHandler.getWindow());
		//Check if any events have been triggered
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader.getID());

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

	if (glfwGetKey(window, GLFW_KEY_1))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (glfwGetKey(window, GLFW_KEY_2))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (glfwGetKey(window, GLFW_KEY_3))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}