#define _CRT_SECURE_NO_WARNINGS
#include "Engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

double Engine::g_lastX				 = 0;
double Engine::g_lastY				 = 0;
bool Engine::g_leftMouseDown		 = 0;
bool Engine::g_rightMouseDown		 = 0;
bool Engine::g_firstMouseEnter		 = 0;
bool Engine::g_mouse_constrain		 = false;
unsigned int Engine::g_running		 = false;

double g_lastTime = glfwGetTime();
double currentTime = glfwGetTime();
int frames = 0;
int fps = 0;
double TPF = 0;
float sleepTime = 0;
int steps = 0;
float simulationTime = 0.0f;
float engineTime = 0.0;
float savedTime = 0.0f;
float timeRatio = 1.0f;
float sum = 0.0f;

WindowHandler* Engine::m_window;
FluidField* Engine::m_fluid;

Engine::Engine()
{
	std::cout << "INITIALIZING::ENGINE" << std::endl;

	m_window = new WindowHandler(c_WIDTH, c_HEIGHT, c_WINDOW_NAME);
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	//Open window
	if (m_window->open() == -1) return;

	//GLAD library loading
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR :: Unable to initialize GLAD";
		return;
	}
	
	g_pc_time = glfwGetTime();
	if (g_fps_limit < 0)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		g_fps_limit = mode->refreshRate;
	}
	//Set the viewport size
	glViewport(0, 0, c_WIDTH, c_HEIGHT);
	//Resize the viewport when the window size is changed
	glfwSetFramebufferSizeCallback(m_window->getWindow(), FRAME_BUFFER_SIZE_CALLBACK);
	glfwSetKeyCallback(m_window->getWindow(), KEY_CALLBACK);
	//Mouse Events
	glfwSetCursorPosCallback(m_window->getWindow(), MOUSE_CALLBACK);
	//Key Events
	glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	m_fluid = new FluidField(c_WIDTH, c_HEIGHT, c_RESOLUTION);

	std::cout << "SUCCESS::INITIALIZATION::ENGINE" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Engine Initialized");
}

void Engine::Run() {
	std::cout << "STARTING::ENGINE" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Starting Engine...");	
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

		physicsUpdate();
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
			std::this_thread::sleep_for(std::chrono::milliseconds((long)(sleepTime)));
		}
	}
	std::cout << "EXITED::RENDER::LOOP" << std::endl;
	return;
}

void Engine::calculateDeltatime()
{
	currentTime = glfwGetTime();
	g_deltaTime = currentTime - g_lastTime;
	g_lastTime = glfwGetTime();
}

void Engine::calculateSleeptime() {
	
	sleepTime = (1.0f / g_fps_limit - g_deltaTime + sleepTime / 1000.0f) * 1000; //ms

	//Avoiding negative numbers
	if (sleepTime < 0 || sleepTime == 0)
	{
		sleepTime = 0;
	}
}

void Engine::calculateFPS() {
	fps = (int) (1000.f / g_deltaTime);
}

void Engine::update()
{
	m_fluid->updateMouse(&g_lastX, &g_lastY, &g_leftMouseDown, &g_rightMouseDown);
	m_fluid->updateConfiguration();

	m_fluid->Draw(glm::vec3(0, 0, 0));
}

void Engine::physicsUpdate() {
	float tpf = g_deltaTime + sleepTime / 1000.0f;

	if (tpf < 0)
	{
		tpf = 0;
		std::cout << "TPF < 0!" << std::endl;
	}

	m_fluid->timeStep(g_deltaTime);
}

void Engine::IO_EVENTS(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		std::cout << "TERMINATING::APPLICATION" << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Terminating...");
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
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

	const float cameraSensitivity = m_camera->sensitivity * g_deltaTime;
	float time = glfwGetTime(); //Seconds
	float passed_time = 0;

	//Step forward a single timestep
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && Engine::g_running == false)
	{
		m_fluid->timeStep(c_precision);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		m_fluid->updateConfiguration();
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		m_fluid->reset();
	}
	//Camera movement
	/*
	//Forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//std::cout << "Forward: { " << m_camera->forward().x << ", " << m_camera->forward().y << ", " << m_camera->forward().z << " }" << std::endl;
		m_camera->processKeyboardInput(Camera_Movement::FORWARD, g_deltaTime);
	}
	//Backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::BACKWARD, g_deltaTime);
	}
	//Left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::LEFT, g_deltaTime);
	}
	//Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::RIGHT, g_deltaTime);
	}
	//Up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::UP, g_deltaTime);
	}
	//Down
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		m_camera->processKeyboardInput(Camera_Movement::DOWN, g_deltaTime);
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

void Engine::saveResults() {
	if (g_running && g_save_result)
	{
		Engine::g_running = false;
		//std::string filename = "-Res" + std::to_string(c_RESOLUTION) + "-dx" + std::to_string((int)(c_precision * 1000)) + "-dt" + std::to_string((int)(g_deltaTime * 1000)) + "-sT" + std::to_string((int)(simulationTime)) + "-hz" + std::to_string((int)g_fps_limit) + "-pcT" + std::to_string((int)currentTime) + "-b" + std::to_string(c_precision_bound) + "-Z" + std::to_string((int)sum);
		std::string filename = "-Res" + std::to_string(c_RESOLUTION) + "-dx" + std::to_string((int)(c_precision * 1000)) + "-dt" + std::to_string((int)(g_deltaTime * 1000));
		std::string path = p_GENERATED_RESULTS + filename + ".png";
		saveImage(path.c_str(), m_window->getWindow());
		g_save_result = false;
		//sum = 0;
	}
}

void Engine::MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos) {
	if (g_mouse_constrain)
	{
		if (xPos <= 10.0)
		{
			glfwSetCursorPos(window, 11.0, yPos);
		}
		else if (xPos >= c_WIDTH - 10.0)
		{
			glfwSetCursorPos(window, c_WIDTH - 11.0, yPos);
		}
		if (yPos <= 10.0)
		{
			glfwSetCursorPos(window, xPos, 11.0);
		}
		else if (yPos >= c_HEIGHT - 10.0)
		{
			glfwSetCursorPos(window, xPos, c_HEIGHT - 11.0);
		}
	}
	if (g_firstMouseEnter)
	{
		g_lastX = xPos;
		g_lastY = yPos;
		g_firstMouseEnter = false;
	}
	float xTravel, yTravel;
	xTravel = xPos - g_lastX;
	yTravel = yPos - g_lastY;
	g_lastX = xPos;
	g_lastY = yPos;
	g_leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	g_rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	//const float sensitivity = m_camera->sensitivity / 100.f;
	//m_camera->processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void Engine::FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	m_fluid->updateViewport(width, height);
}

void Engine::KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_PAUSE || key == GLFW_KEY_0) && action == GLFW_PRESS)
	{
		std::cout << "Running: " << g_running << std::endl;
		Engine::g_running = !g_running;
	}
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		if (m_window->windowState == WindowHandler::WindowState::WINDOWED) {
			m_window->setState(WindowHandler::WindowState::FULLSCREEN);
		}
		else if (m_window->windowState == WindowHandler::WindowState::FULLSCREEN) {
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
