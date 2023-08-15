#define _CRT_SECURE_NO_WARNINGS
#include "Engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

/*double Engine::g_mouse.lastX				 = 0;
double Engine::g_mouse.lastY				 = 0;
bool Engine::g_mouse.leftMouseDown		 = 0;
bool Engine::g_mouse.rightMouseDown		 = 0;
bool Engine::g_firstMouseEnter		 = 0;
bool Engine::g_mouse_constrain		 = true;*/
bool Engine::g_running				 = false;

double g_lastTime = glfwGetTime();
double currentTime = glfwGetTime();
int frames = 0;
int fps = 0;
double TPF = 0;
double sleepTime = 0;
int steps = 0;
float simulationTime = 0.0f;
float engineTime = 0.0;
float savedTime = 0.0f;
float timeRatio = 1.0f;
float sum = 0.0f;

WindowHandler* Engine::m_window;
FluidSimulation* Engine::m_fluid;
Engine::Mouse Engine::g_mouse;
double Engine::Time::deltaTime, Engine::Time::fixedDeltaTime, Engine::Time::pc_time;

Engine::Engine()
{
	std::cout << "INITIALIZING::ENGINE" << std::endl;

	m_window = new WindowHandler(c_WIDTH, c_HEIGHT, c_WINDOW_NAME);
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	//Open window
	if (m_window->open() == -1) return;

	int w, h, channels;
	unsigned char* data = stbi_load(p_APPLICATION_ICON, &w, &h, &channels, 0);
	if (data != NULL)
	{

		GLFWimage* icon = new GLFWimage();
		icon->height = h;
		icon->width = w;
		icon->pixels = data;
		glfwSetWindowIcon(m_window->getWindow(), 1, icon);
		delete(icon);
		icon = nullptr;
	}
	else
	{
		std::cout << "Could not set window icon" << std::endl;
	}
	stbi_image_free(data);

	//GLAD library loading
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "ERROR :: Unable to initialize GLAD";
		return;
	}

	Time::pc_time = glfwGetTime();
	if (g_fps_limit < 0)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		g_fps_limit = mode->refreshRate;
	}
	//Set the viewport size
	glViewport(0, 0, c_WIDTH, c_HEIGHT);
	//Resize the viewport when the window size is changed
	glfwSetFramebufferSizeCallback(m_window->getWindow(), FRAMEBUFFER_RESIZE_CALLBACK);
	glfwSetWindowIconifyCallback(m_window->getWindow(), WINDOW_ICONIFY_CALLBACK);
	glfwSetWindowFocusCallback(m_window->getWindow(), WINDOW_FOCUS_CALLBACK);
	glfwSetKeyCallback(m_window->getWindow(), KEY_CALLBACK);
	glfwSetCursorPosCallback(m_window->getWindow(), MOUSE_CALLBACK);

	glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	m_fluid = new FluidSimulation(c_WIDTH, c_HEIGHT, c_RESOLUTION);

	std::cout << "SUCCESS::INITIALIZATION::ENGINE" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Engine Initialized");
}

void Engine::Run()
{
	std::cout << "STARTING::ENGINE" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Starting Engine...");

	glfwMaximizeWindow(m_window->getWindow());
	//This removes the FPS cap
	glfwSwapInterval(0);

	std::cout << "ENTERING::RENDER::LOOP" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Fluid Simulation");

	Engine::g_running = true;

	//Rendering loop
	while (!glfwWindowShouldClose(m_window->getWindow()))
	{
		IO_EVENTS(m_window->getWindow());

		calculateDeltatime();
		calculateSleeptime();
		calculateFPS();

		glClearColor(c_DEFAULT_CLEAR_COLOR[0], c_DEFAULT_CLEAR_COLOR[1], c_DEFAULT_CLEAR_COLOR[2], c_DEFAULT_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT);

		if (Engine::g_running)
		{
			physicsUpdate();
		}
		update();

		glBindVertexArray(0);

		/*Double buffer
			When rendering, the front buffer contains the final output of an image and is rendered to the screen.
			While it is being drawn to the screen a back buffer is being drawn behind the scenes in order to reduce flickering issues.
		*/
		//Swap color buffer in order to render new images
		glfwSwapBuffers(m_window->getWindow());
		//Check if any events have been triggered
		glfwPollEvents();

		if (sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((long) (sleepTime)));
		}
	}
	std::cout << "EXITED::RENDER::LOOP" << std::endl;
	return;
}

void Engine::calculateDeltatime()
{
	currentTime = glfwGetTime();
	Time::deltaTime = currentTime - g_lastTime;
	g_lastTime = glfwGetTime();
}

void Engine::calculateSleeptime()
{

	sleepTime = (1.0f / g_fps_limit - Time::deltaTime + sleepTime / 1000.0f) * 1000; //ms

	//Avoiding negative numbers
	if (sleepTime < 0 || sleepTime == 0)
	{
		sleepTime = 0;
	}
}

void Engine::calculateFPS()
{
	fps = (int) (1000.f / Time::deltaTime);
}

void Engine::update()
{
	m_fluid->updateMouse(&g_mouse.lastX, &g_mouse.lastY, &g_mouse.leftMouseDown, &g_mouse.rightMouseDown);
	m_fluid->updateConfiguration();

	m_fluid->Draw(glm::vec3(0, 0, 0));
}

void Engine::physicsUpdate()
{
	m_fluid->timeStep((float) Time::deltaTime);
}

void Engine::IO_EVENTS(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		std::cout << "TERMINATING::APPLICATION" << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Terminating...");
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE)
		{
			saveImage("C:/Users/to9751/Pictures/Generated Images/Framebuffer.png", window);
		}
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

	const float cameraSensitivity = m_camera->sensitivity * (float) Time::deltaTime;
	double time = glfwGetTime(); //Seconds
	float passed_time = 0;

	g_mouse.leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	g_mouse.rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	if (g_mouse.leftMouseDown || g_mouse.rightMouseDown)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		constrainMouse(window, mouseX, mouseY);
	}

	//Step forward a single timestep
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && Engine::g_running == false)
	{
		m_fluid->timeStep(c_precision);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		m_fluid->updateConfiguration();
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		m_fluid->reset();
	}
	//Camera movement
	/*
	//Forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//std::cout << "Forward: { " << m_camera->forward().x << ", " << m_camera->forward().y << ", " << m_camera->forward().z << " }" << std::endl;
		m_camera->processKeyboardInput(Camera_Movement::FORWARD, Time::deltaTime);
	}
	//Backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::BACKWARD, Time::deltaTime);
	}
	//Left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::LEFT, Time::deltaTime);
	}
	//Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::RIGHT, Time::deltaTime);
	}
	//Up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::UP, Time::deltaTime);
	}
	//Down
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::DOWN, Time::deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f) * -cameraSensitivity);
	}
	*/
}

//This is some copy pasta from somewhere on stackoverflow
void Engine::saveImage(const char* path, GLFWwindow* window)
{
	std::cout << "Writing file..." << std::endl;
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	int channelAmount = 3;
	int stride = channelAmount * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	int bufferSize = stride * height;
	std::vector<char> buffer(bufferSize);

	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path, width, height, channelAmount, buffer.data(), stride);
	std::cout << "File has been written at " << path << std::endl;
}

void Engine::saveResults()
{
	if (g_running && g_save_result)
	{
		Engine::g_running = false;
		//std::string filename = "-Res" + std::to_string(c_RESOLUTION) + "-dx" + std::to_string((int)(c_precision * 1000)) + "-dt" + std::to_string((int)(Time::deltaTime * 1000)) + "-sT" + std::to_string((int)(simulationTime)) + "-hz" + std::to_string((int)g_fps_limit) + "-pcT" + std::to_string((int)currentTime) + "-b" + std::to_string(c_precision_bound) + "-Z" + std::to_string((int)sum);
		std::string filename = "-Res" + std::to_string(c_RESOLUTION) + "-dx" + std::to_string((int) (c_precision * 1000)) + "-dt" + std::to_string((int) (Time::deltaTime * 1000));
		std::string path = p_GENERATED_RESULTS + filename + ".png";
		saveImage(path.c_str(), m_window->getWindow());
		g_save_result = false;
		//sum = 0;
	}
}

void Engine::constrainMouse(GLFWwindow* window, double xPos, double yPos)
{
	double margin = 2.f;
	if (xPos <= margin)
	{
		glfwSetCursorPos(window, margin + 1, yPos);
	}
	else if (xPos >= c_WIDTH - margin)
	{
		glfwSetCursorPos(window, c_WIDTH - margin + 1, yPos);
	}
	if (yPos <= margin)
	{
		glfwSetCursorPos(window, xPos, margin + 1);
	}
	else if (yPos >= c_HEIGHT - margin)
	{
		glfwSetCursorPos(window, xPos, c_HEIGHT - margin + 1);
	}
}

void Engine::MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos)
{
	if (g_mouse.firstMouseEnter)
	{
		g_mouse.lastX = xPos;
		g_mouse.lastY = yPos;
		g_mouse.firstMouseEnter = false;
	}
	double xTravel, yTravel;
	xTravel = xPos - g_mouse.lastX;
	yTravel = yPos - g_mouse.lastY;
	g_mouse.lastX = xPos;
	g_mouse.lastY = yPos;
	//const float sensitivity = m_camera->sensitivity / 100.f;
	//m_camera->processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void Engine::Pause()
{
	Engine::g_running = !Engine::g_running;
	std::cout << "Running: " << Engine::g_running << std::endl;
}

void Engine::FRAMEBUFFER_RESIZE_CALLBACK(GLFWwindow* window, int width, int height)
{
	if (width <= 0 || height <= 0 || glfwGetWindowAttrib(window, GLFW_ICONIFIED)) return;
	std::cout << "Width: " << width << " Height: " << height << std::endl;
	glViewport(0, 0, width, height);
	m_fluid->resizeViewport(width, height);
}

void Engine::WINDOW_ICONIFY_CALLBACK(GLFWwindow* window, int iconified)
{
	if (Engine::g_running && iconified) glfwRequestWindowAttention(window);
}

void Engine::WINDOW_FOCUS_CALLBACK(GLFWwindow* window, int focused)
{
	if (focused)
	{
//m_window->setState(WindowHandler::WindowState::FULLSCREEN);
	}
	else if (!focused)
	{
		m_window->setState(WindowHandler::WindowState::WINDOWED);
	}
}

void Engine::KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_PAUSE || key == GLFW_KEY_0) && action == GLFW_PRESS)
	{
		Engine::Pause();
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (m_window->windowState == WindowHandler::WindowState::WINDOWED)
		{
			m_window->setState(WindowHandler::WindowState::FULLSCREEN);
		}
		else if (m_window->windowState == WindowHandler::WindowState::FULLSCREEN)
		{
			m_window->setState(WindowHandler::WindowState::WINDOWED);
		}
	}
	//Dye
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(1);
	}
	//Velocity
	else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(2);
	}
	//Divergence
	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(3);
	}
	//Pressure
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(4);
	}
	//Curl
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(5);
	}
	//Temperature
	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(6);
	}
	//Density
	else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(7);
	}
}
