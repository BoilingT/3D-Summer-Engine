#include "WindowHandler.h"

int WindowHandler::open()
{

	window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "ERROR::CREATE::WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwRequestWindowAttention(window);
	std::cout << "SUCCESS::INITIALIZATION::WINDOW" << std::endl;
	return 0;
}

int WindowHandler::setState(WindowState state)
{

	if (monitor == nullptr) return -1;

	if (state == WindowState::WINDOWED)
	{

		glfwSetWindowMonitor(window, NULL, 0, 0, WIDTH, HEIGHT, GLFW_DONT_CARE);
		glfwMaximizeWindow(window);
	}
	else if (state == WindowState::FULLSCREEN)
	{
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	windowState = state;

	return 0;
}