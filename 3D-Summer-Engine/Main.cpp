#include <iostream>
#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#include "WindowHandler.h"

const int WIDTH = 1000;
const int HEIGHT = 700;
const char* WINDOW_NAME = "Summer-Engine";

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {

	std::cout << "Engine Starting..." << std::endl;
	
	WindowHandler windowHandler(WIDTH, HEIGHT, WINDOW_NAME);

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

	std::cout << "Engine Started" << std::endl;
	
	//Draw
	while (!glfwWindowShouldClose(windowHandler.getWindow())) 
	{
		processInput(windowHandler.getWindow());

		glClearColor(0.58f, 0.71f, 0.91, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(windowHandler.getWindow());
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