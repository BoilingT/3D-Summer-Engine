#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class WindowHandler
{
private:
	const int WIDTH;
	const int HEIGHT;
	const char* WINDOW_NAME;
	GLFWwindow* window;
public:
	WindowHandler(int width, int height, const char* window_name) : WIDTH(width), HEIGHT(height), WINDOW_NAME(window_name) {
		window = nullptr;
	}

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

	int init();
};

