#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class WindowHandler
{
private:
	int width;
	int height;
	std::string title;
	const GLFWvidmode* mode;
	GLFWwindow* window;
	GLFWmonitor* monitor;
public:
	enum class WindowState { WINDOWED, FULLSCREEN, NONE };

	WindowState windowState;

	WindowHandler()
	{
		// Do nothing
	}

	int init(std::string _title);
	int setState(WindowState state);
	int setState(WindowState state, unsigned int _width, unsigned int _height);
	int setIcon(std::string path);
	void setTitle(std::string title);
	int open(WindowState state, unsigned int _width, unsigned int _height);
	int open(WindowState state);

	~WindowHandler()
	{
		std::cout << "DESTROYED::WINDOW" << std::endl;
	}

	int getWidth()
	{
		return width;
	}

	int getHeight()
	{
		return height;
	}

	std::string getWindowName()
	{
		return title;
	}

	GLFWwindow* getWindow()
	{
		return window;
	}
};

