#include "WindowHandler.h"
#include <stb/stb_image.h>

int WindowHandler::init(std::string _title)
{
	title = _title;
	window = nullptr;

	std::cout << "INITIALIZING::WINDOW" << std::endl;

	int success = glfwInit();
	if (!success) return -1;

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

int WindowHandler::open(WindowState state)
{
	return open(state, mode->width, mode->height);
}

int WindowHandler::open(WindowState state, unsigned int _width, unsigned int _height)
{
	if (window != NULL || (state == WindowState::NONE && (_width == 0 || _height == 0))) return -1;

	windowState = state;
	width = _width;
	height = _height;

	std::cout << "Opened window: Height: " << height << " Width: " << width << std::endl;

	if (state == WindowState::WINDOWED)
	{
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		glfwMaximizeWindow(window);
	}
	else if (state == WindowState::FULLSCREEN)
	{
		window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
	}
	else
	{
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	}

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
	return setState(state, mode->width, mode->height);
}

int WindowHandler::setState(WindowState state, unsigned int _width, unsigned int _height)
{
	if (monitor == nullptr || window == NULL || (state == WindowState::NONE && (_width == 0 || _height == 0))) return -1;

	windowState = state;
	width = _width;
	height = _height;

	std::cout << "Set window to: Height: " << height << " Width: " << width << std::endl;

	if (state == WindowState::WINDOWED)
	{
		glfwSetWindowMonitor(window, NULL, 0, 0, width, height, GLFW_DONT_CARE);
		glfwMaximizeWindow(window);
	}
	else if (state == WindowState::FULLSCREEN)
	{
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}

	return 0;
}

void WindowHandler::setTitle(std::string title)
{
	glfwSetWindowTitle(window, title.c_str());
}

int WindowHandler::setIcon(std::string path)
{
	if (window == NULL) return -1;
	int w, h, channels;
	unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
	if (data != NULL)
	{

		GLFWimage* icon = new GLFWimage();
		icon->height = h;
		icon->width = w;
		icon->pixels = data;
		glfwSetWindowIcon(window, 1, icon);
		delete(icon);
		icon = nullptr;
		stbi_image_free(data);
		return 0;
	}
	else if (data == NULL)
	{
		return -1;
	}
}
