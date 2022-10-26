#include "Engine.h"

void Engine::Init()
{
	//Open window
	if (_window->init() == -1) {
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
	glfwSetFramebufferSizeCallback(_window->getWindow(), FRAME_BUFFER_SIZE_CALLBACK);
	//Mouse Events
	glfwSetCursorPosCallback(_window->getWindow(), MOUSE_CALLBACK);
	//Key Events
	glfwSetInputMode(_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	_compute_shader = new Compute(p_COMPUTE_SHADER, glm::vec2(10.0f, 1.0f));
}

void Engine::Run() {
	std::cout << "Starting Engine..." << std::endl;
	glfwSetWindowTitle(_window->getWindow(), "Starting Engine...");

	int frames = 0;
	int fps = 0;
	float sleepTime = 0;
	
	//Setup shader
	Shader shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER);
	_shader = &shader;
	_shader->use();
	Plane plane(glm::vec3(0.0f), glm::vec3(3.0f), glm::vec3(0.0f));
	FluidField fluid(c_WIDTH, c_HEIGHT, 256*256);

	//glm::vec3 o = glm::vec3(-c_WIDTH / 2.f, c_HEIGHT / 2.f, 0);
	glm::vec3 o = glm::vec3(0, 0, 0);
	
	std::cout << "Started rendering loop..." << std::endl;
	glfwSetWindowTitle(_window->getWindow(), "Started rendering loop...");

	lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(_window->getWindow()))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		frames++;

		if (deltaTime >= 1.f)
		{
			//double TPF = 1000.0 / (double)frames;
			//std::cout << TPF << "ms/frame" << std::endl;
			double TPF = 1000.0 / (double)frames;
			//sleepTime = 1000.f / 144.f - TPF;
			fps = frames/deltaTime;
			frames = 0;
			lastTime += 1.f;
			std::string title = "FPS: " + std::to_string(fps) + " TPF: " + std::to_string(TPF) + "ms";
			glfwSetWindowTitle(_window->getWindow(), title.c_str());
		}

		POLL_EVENTS(_window->getWindow());

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

		//viewM = _camera->lookAt(_camera->forward());

		//viewM = glm::lookAt(_camera->getPos(), _camera->getPos() + _camera->forward(), _camera->up());
		_shader->setMat4f("view", viewM);

		//projectionM = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
		//projectionM = glm::ortho(-(float)c_WIDTH / 2, (float)c_WIDTH / 2, -(float)c_HEIGHT / 2, (float)c_HEIGHT / 2, -1000.0f, 1000.0f);
		projectionM = glm::ortho(0.0f, (float)c_WIDTH, 0.0f, (float)c_HEIGHT, -1000.0f, 1000.0f);
		_shader->setMat4f("projection", projectionM);

		float timeValue = glfwGetTime();
		float val = sin(timeValue / 2);
		
		fluid.DrawCellField(o, _shader);

		int vertexColorLocation = glGetUniformLocation(_shader->getID(), "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, val, timeValue, 1.0f);

		glBindVertexArray(0);

		/*Double buffer
			When rendering, the front buffer contains the final output of an image and is rendered to the screen.
			While it is being drawn to the screen a back buffer is being drawn behind the scenes in order to reduce flickering issues.
		*/
		//Swap color buffer in order to render new images
		glfwSwapBuffers(_window->getWindow());
		//Check if any events have been triggered
		glfwPollEvents();

		if (sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((long)(sleepTime)));
		}
	}
	glDeleteProgram(shader.getID());
	return;
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

void Engine::POLL_EVENTS(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
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

	const float cameraSensitivity = _camera->sensitivity * deltaTime;

	//Forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//std::cout << "Forward: { " << _camera->forward().x << ", " << _camera->forward().y << ", " << _camera->forward().z << " }" << std::endl;
		_camera->processKeyboardInput(Camera_Movement::FORWARD, deltaTime);
	}
	//Backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		_camera->processKeyboardInput(Camera_Movement::BACKWARD, deltaTime);
	}
	//Left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		_camera->processKeyboardInput(Camera_Movement::LEFT, deltaTime);
	}
	//Right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		_camera->processKeyboardInput(Camera_Movement::RIGHT, deltaTime);
	}
	//Up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		_camera->processKeyboardInput(Camera_Movement::UP, deltaTime);
	}
	//Down
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		_camera->processKeyboardInput(Camera_Movement::DOWN, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		_camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		_camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		_camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f) * cameraSensitivity);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		_camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f) * -cameraSensitivity);
	}
}

void Engine::MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos) {
	/*if (firstMouseEnter)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouseEnter = false;
	}
	float xTravel, yTravel;
	xTravel = xPos - lastX;
	yTravel = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	*/
	//const float sensitivity = _camera->sensitivity / 100.f;
	//_camera->processMouseMovement(yTravel * -sensitivity, xTravel * -sensitivity);
}

void Engine::FRAME_BUFFER_SIZE_CALLBACK(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
