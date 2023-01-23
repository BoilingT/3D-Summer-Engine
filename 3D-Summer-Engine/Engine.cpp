#define _CRT_SECURE_NO_WARNINGS
#include "Engine.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

double Engine::g_lastX				 = 0;
double Engine::g_lastY				 = 0;
bool Engine::g_mouseDown			 = 0;
bool Engine::g_firstMouseEnter		 = 0;
bool Engine::g_mouse_constrain		 = true;
unsigned int Engine::g_running		 = false;

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
	//glEnable(GL_DEPTH_TEST);
	g_pc_time = glfwGetTime();
	if (g_fps_limit <= 0)
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
			that form a primitive form. It has the ability to use the primitive to generateFrom other shapes by making new vertices to form new or different
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
void Engine::Run() {
	std::cout << "STARTING::ENGINE" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Starting Engine...");

	float g_lastTime = glfwGetTime();
	float g_lastTime2 = g_lastTime;
	int frames = 0;
	int fps = 0;
	double TPF = 0;
	float averageDT = 0;
	float dtTotal = 0;
	float sleepTime = 0;
	double currentTime = glfwGetTime();
	int steps = 0;
	glfwSwapInterval(0);
	//Rect plane(glm::vec3(0.0f), glm::vec3(3.0f), glm::vec3(0.0f));

	//glm::vec3 origin = glm::vec3(-c_WIDTH / 2.f, c_HEIGHT / 2.f, 0);

	glm::vec3 origin = glm::vec3(0, 0, 0);
	
	std::cout << "ENTERING::RENDER::LOOP" << std::endl;
	glfwSetWindowTitle(m_window->getWindow(), "Started rendering loop...");

	Engine::g_running = !glfwWindowShouldClose(m_window->getWindow());
	
	float simulationTime = 0.0f;
	float engineTime = 0.0;
	float savedTime = 0.0f;
	float timeRatio = 1.0f;
	while (!glfwWindowShouldClose(m_window->getWindow()))
	{
		currentTime = glfwGetTime();
		g_deltaTime = currentTime - g_lastTime - sleepTime/1000.0f;
		g_lastTime = glfwGetTime();
		frames++;
		dtTotal += g_deltaTime;

		sleepTime = (1.0f / g_fps_limit - g_deltaTime) * 1000; //ms

		if (sleepTime < 0 || g_fps_limit <= 0)
		{
			sleepTime = 0;
		}

		std::string title = "FPS: " + std::to_string(fps) + " dT: " + std::to_string(g_deltaTime*1000.0f) + "ms avg dT: " + std::to_string(averageDT*1000.0f) + "ms TPF: " + std::to_string(TPF) + " time ratio: " + std::to_string(timeRatio);
		if (currentTime - g_lastTime2 >= 1.f)
		{
			//double TPF = 1000.0 / (double)frames;
			//std::cout << TPF << "ms/frame" << std::endl;
			TPF = 1000.0 / (double)frames;
			fps = frames/(currentTime - g_lastTime2-sleepTime/1000.0f);
			averageDT = (double)dtTotal/(double)frames;
			frames = 0;
			dtTotal = 0;
			//sleepTime = 1000.f / 60.0f -(currentTime - g_lastTime2) * 2.0f;
			g_lastTime2 += 1.f;

			if (engineTime > 0)
			{
				timeRatio = simulationTime / engineTime;
			}
			std::cout << "Simulation: " << simulationTime << "s Engine: " << engineTime << "s PC: " << currentTime - g_pc_time << "s Steps: " << steps << " Saved: " << savedTime*1000 << "ms" << std::endl;
			
			glfwSetWindowTitle(m_window->getWindow(), title.c_str());
		}
		if (simulationTime >= 10 && g_running)
		{
			if (!g_result_saved && g_save_result)
			{
				std::string filename = "-Res" + std::to_string(c_RESOLUTION) + "-dx" + std::to_string((int)(c_precision * 1000)) + "-dt" + std::to_string((int)(g_deltaTime * 1000)) + "-sT" + std::to_string((int)(simulationTime)) + "-hz" + std::to_string((int)g_fps_limit) + "-pcT" + std::to_string((int)currentTime) + "-b" + std::to_string(c_precision_bound);
				std::string path = "C:/Users/tobbe/Pictures/simulated flow/result" + filename + ".png";
				saveImage(path.c_str(), m_window->getWindow());
				g_result_saved = true;
				Engine::g_running = false;
			}
			engineTime = 0.0f;
			simulationTime = 0.0f;
		}

		IO_EVENTS(m_window->getWindow());

		//Render
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(c_DEFAULT_CLEAR_COLOR[0], c_DEFAULT_CLEAR_COLOR[1], c_DEFAULT_CLEAR_COLOR[2], c_DEFAULT_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT);

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

		double timeValue = glfwGetTime();
		double val = sin(timeValue / 2);
		
		m_fluid->updateMouse(&g_lastX, &g_lastY, &g_mouseDown);
		double tpf = g_deltaTime + sleepTime / 1000.0f;
		if (tpf < 0)
		{
			tpf = 0;
			std::cout << "TPF < 0!" << std::endl;
		}
		//Step forward in time until it has accounted for the number of steps lost by lag
		if (Engine::g_running)
		{
			if (c_precision_bound)
			{
				if (tpf > c_precision) //The engine is running slower than the simulator
				{
					float ratio = (tpf) / c_precision;
					steps = ratio;
					float c = 1 + (ratio - steps)/steps;
					savedTime = 0;
					for (unsigned int i = 0; i < steps; i++)
					{
						m_fluid->timeStep(c_precision * c);
						simulationTime += c_precision * c;
						if (i > 0)
						{
							savedTime += c_precision * c;
						}
					}
					//simulationTime += c_precision * c * steps;
				}
				else //The engine is running faster than the simulator
				{
					float ratio = tpf / c_precision;  //How much faster the simulation is than the engine
					//Calulate how much faster the simulator is going measured in steps
					//m_fluid->timeStep(c_precision);
					m_fluid->timeStep(c_precision * ratio);
					simulationTime += c_precision * ratio;
					//simulationTime += tpf;
				}
			}
			else
			{
				m_fluid->timeStep(c_precision);
				simulationTime += c_precision;
				//m_fluid->timeStep(tpf);
			}
			engineTime += tpf;
		}
		else
		{
			engineTime = 0;
			simulationTime = 0;
		}
		m_fluid->Draw(origin);
		//m_fluid->DrawCellField(origin);
		
		//int vertexColorLocation = glGetUniformLocation(m_primary_shader->getID(), "ourColor");
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
	std::cout << "EXITED::RENDER::LOOP" << std::endl;
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
	float time = glfwGetTime(); //Seconds
	float passed_time = 0;

	//Dye
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(1);
	}
	//Velocity
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(2);
	}
	//Divergence
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(3);
	}
	//Pressure
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(4);
	}
	//Curl
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(5);
	}
	//Temperature
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(6);
	}
	//Density
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		m_fluid->swapBuffer(7);
	}
	//Step forward a single timestep
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && Engine::g_running == false)
	{
		m_fluid->timeStep(c_precision);
	}

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
	g_mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	//const float sensitivity = m_camera->sensitivity / 100.f;
	//m_camera->processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void Engine::FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Engine::KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_PAUSE || key == GLFW_KEY_0) && action == GLFW_PRESS)
	{
		std::cout << "Running: " << g_running << std::endl;
		Engine::g_running = !g_running;
	}
}
