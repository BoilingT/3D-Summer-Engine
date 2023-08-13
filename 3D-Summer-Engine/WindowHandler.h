#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class WindowHandler
{
private:
	const int WIDTH;
	const int HEIGHT;
	const char* WINDOW_NAME;
	const GLFWvidmode* mode;
	GLFWwindow* window;
	GLFWmonitor* monitor;
public:
	enum class WindowState { WINDOWED, FULLSCREEN };

	WindowState windowState;

	WindowHandler(int width, int height, const char* window_name) : WIDTH(width), HEIGHT(height), WINDOW_NAME(window_name) {
		window = nullptr;
		std::cout << "INITIALIZING::WINDOW" << std::endl;
		
		glfwInit();

		windowState = WindowState::WINDOWED;
		monitor = glfwGetPrimaryMonitor();
		mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
		glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
		//glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		//glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		//glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		//glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	}
	int setState(WindowState state);
	int open();

	~WindowHandler() {
		std::cout << "DESTROYED::WINDOW" << std::endl;
	}

	int getWidth() {
		return WIDTH;
	}

	int getHeight() {
		return HEIGHT;
	}

	const char* getWindowName() {
		return WINDOW_NAME;
	}

	GLFWwindow* getWindow() {
		return window;
	}
};

