#define _CRT_SECURE_NO_WARNINGS
#include "Engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Engine::Init()
{
	//Open window
	if (m_window->init() == -1) {
		return;
	}

	//GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR :: Unable to initialize GLAD";
		return;
	}
	
	glEnable(GL_DEPTH_TEST);

	//Set the viewport size
	glViewport(0, 0, c_WIDTH, c_HEIGHT);
	//Resize the viewport when the window size is changed
	glfwSetFramebufferSizeCallback(m_window->getWindow(), FRAME_BUFFER_SIZE_CALLBACK);
	//Mouse Events
	glfwSetCursorPosCallback(m_window->getWindow(), MOUSE_CALLBACK);
	//Key Events
	glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_fluid = new FluidField(c_WIDTH, c_HEIGHT, c_RESOLUTION);
}

void Engine::Run() {
	std::cout << "Starting Engine..." << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Starting Engine...");

	int frames = 0;
	int fps = 0;
	float sleepTime = 0;
	
	//Rect plane(glm::vec3(0.0f), glm::vec3(3.0f), glm::vec3(0.0f));

	//glm::vec3 origin = glm::vec3(-c_WIDTH / 2.f, c_HEIGHT / 2.f, 0);
	glm::vec3 origin = glm::vec3(0, 0, 0);
	
	std::cout << "Started rendering loop..." << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Started rendering loop...");
	
	g_lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(m_window->getWindow()))
	{
		float currentTime = glfwGetTime();
		g_deltaTime = currentTime - g_lastTime;
		frames++;

		if (g_deltaTime >= 1.f)
		{
			//double TPF = 1000.0 / (double)frames;
			//std::cout << TPF << "ms/frame" << std::endl;
			double TPF = 1000.0 / (double)frames;
			//sleepTime = 1000.f / 144.f - TPF;
			fps = frames/g_deltaTime;
			frames = 0;
			g_lastTime += 1.f;
			std::string title = "FPS: " + std::to_string(fps) + " TPF: " + std::to_string(TPF) + "ms";
			glfwSetWindowTitle(m_window->getWindow(), title.c_str());
		}

		IO_EVENTS(m_window->getWindow());

		//Render
		glClearColor(c_DEFAULT_CLEAR_COLOR[0], c_DEFAULT_CLEAR_COLOR[1], c_DEFAULT_CLEAR_COLOR[2], c_DEFAULT_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Transform Matrices
		/*
			Local Space -> (Model Matrix) -> World Space -> (View Matrix) -> View Space -> (Projection Matrix) -> Clip Space -> (Viewport Transform) -> Screen Space
			Clip Space = Projection Matrix * View Matrix * Model Matrix * Local Vector
			Projection: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
		*/

		glm::mat4 modelM = glm::mat4(1.0f);
		glm::mat4 viewM = glm::mat4(1.0f);
		glm::mat4 projectionM = glm::mat4(1.0f);

		//viewM = m_camera->lookAt(m_camera->forward());

		//viewM = glm::lookAt(m_camera->getPos(), m_camera->getPos() + m_camera->forward(), m_camera->up());

		//projectionM = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
		//projectionM = glm::ortho(-(float)c_WIDTH / 2, (float)c_WIDTH / 2, -(float)c_HEIGHT / 2, (float)c_HEIGHT / 2, -1000.0f, 1000.0f);
		////projectionM = glm::ortho(0.0f, (float)c_WIDTH, 0.0f, (float)c_HEIGHT, -1000.0f, 1000.0f);

		float timeValue = glfwGetTime();
		float val = sin(timeValue / 2);
		
		m_fluid->Draw(origin);
		//m_fluid->DrawCellField(origin);
		
		//int vertexColorLocation = glGetUniformLocation(m_shader->getID(), "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, val, timeValue, 1.0f);

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
	return;
}

void Engine::saveImage(const char* path, GLFWwindow* window)
{
	std::cout << "Writing file..." << std::endl;
	int width											 = 0;
	int height											 = 0;
	glfwGetFramebufferSize(window, &width, &height);
	int channelAmount									 = 3;
	int stride											 = channelAmount * width;
	stride												 += (stride % 4) ? (4 - stride % 4) : 0;
	int bufferSize										 = stride * height;
	std::vector<char> buffer(bufferSize);

	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path, width, height, channelAmount, buffer.data(), stride);
	std::cout << "File has been written at " << path << std::endl;
}

/*
	Graphics Pipeline:
		Vertex data -> Vertex Shader -> Shape Assembly (Opt) -> Geometry Shader -> Rasterization (Opt) -> Fragment Shader -> Tests and Blending (Opt)
		Essentials: Vertex data -> Vertex Shader -> Geometry Shader -> Fragent Shader

		Vertex Shader:
			The vertex shader takes a single vertex as data input. Its main purpose is to transform 3D coordinates into different 3D coordinates.
			It also allows for basic processing on the vertex attributes.

		Shape Assembly:
			The assembly stage uses the data from all vertices to create a primitve form and "assembles all the point(s) in the primitive shape given",
			as for an example a triangle.

		Geometry Shader:
			The data collection from the assembly stage is transferred onto the geometry shader. Its job is to take a collection of vertices
			that form a primitive form. It has the ability to use the primitive to generate other shapes by making new vertices to form new or different
			primitives.

		Rasterization:
			The data from the Geometry Shader is passed on to the Rasterization stage where it maps the resulting primitives
			to the corresponding pixels on the screen. This results in fragments for the fragment shader to use. Before the fragment shaders run
			fragments that are outside the screen are clipped and discarded which improves the performance.

		Fragment Shader:
			A Fragments Shader's main purpose is to calculate the final color of a pixel. During this stage is usally when all
			the advanced OpenGL effects occur. The Fragment Shader usally contains data bout the 3D scene that it can use to calculate the final pixel color.

		Tests and Blending
			After all the color values have been determined the final object will then go through the "Alpha test" and "Blending stage". This stage checks the
			depth values of the fragment to determine if a fragment is in front or behind another object and should thus be discarded accordingly.
	*/

void Engine::IO_EVENTS(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		std::cout << "Terminating..." << std::endl;
		glfwSetWindowTitle(m_window->getWindow(), "Terminating...");
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
			saveImage("C:/Users/to9751/Pictures/Generated Images/Framebuffer.png", window);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 144);
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
}

void Engine::MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos) {
	/*if (g_firstMouseEnter)
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
	*/
	//const float sensitivity = m_camera->sensitivity / 100.f;
	//m_camera->processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void Engine::FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
