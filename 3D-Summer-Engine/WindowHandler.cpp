#include "WindowHandler.h"

int WindowHandler::init() {
	std::cout << "INITIALIZING::WINDOW" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "ERROR::CREATE::WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	std::cout << "SUCCESS::INITIALIZATION::WINDOW" << std::endl;
	return 0;
}