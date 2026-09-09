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

	bool init(std::string _title);
	bool setState(WindowState state);
	bool setState(WindowState state, unsigned int _width, unsigned int _height);
	bool setIcon(std::string path);
	void setTitle(std::string title);
	bool open(WindowState state, unsigned int _width, unsigned int _height);
	bool open(WindowState state);

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

