#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

//GLM
#include "glm_includes.h"

#include <glad/glad.h> //Needs to be included before GLFW
#include <GLFW/glfw3.h>
#include "WindowHandler.h"
#include "fileHandler.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "object_includes.h"
#include "FluidSimulation.h"
#include "Compute.h"


class Engine
{
private:
	//Mouse properties
	static struct Mouse
	{
		double lastX = 0;
		double lastY = 0;
		bool leftMouseDown = 0;
		bool rightMouseDown = 0;
		bool firstMouseEnter = false;
	} g_mouse;

	//Fluid simulation properties
	static bool g_running;

	static WindowHandler m_window;
	static FluidSimulation* m_fluid;
	static FileHandler fileSystem;

	WindowHandler::WindowState windowType = WindowHandler::WindowState::WINDOWED;
	Camera m_camera;

	//Window Properties
	const unsigned int c_WIDTH								 = 1920;
	const unsigned int c_HEIGHT								 = 1080;
	const char* c_WINDOW_NAME								 = "Summer Engine";
	const float c_DEFAULT_CLEAR_COLOR[4]					 = { 1.0f, 0.0f, 0.0f, 1.0f };
	const float c_CLEAR_COLOR[4]							 = { 0.28f, 0.41f, 0.61f, 1.0f };

	//Filepaths
	//Note: Only p_GENERATED_RESULTS is being used
	const char* p_DEFAULT_VERTEX_SHADER						 = "Shaders/default_vertex_shader.vert";
	const char* p_DEFAULT_FRAGMENT_SHADER					 = "Shaders/default_fragment_shader.frag";
	const char* p_CONTAINER_IMAGE							 = "Images/LearnOpenGL/container.jpg";
	const char* p_AWESOMEFACE_IMAGE							 = "Images/LearnOpenGL/awesomeface.png";
	const char* p_GENERATED_RESULTS							 = "Images/Generated Results/";
	const char* p_APPLICATION_ICON							 = "./Images/Icon/letter-f.png";


	//Engine variables
	bool  g_save_result									 = false;			//If set to TRUE a screenshot will be saved after the desired time below
	const double g_save_result_time						 = 20.0f;			//How long the simulation has to be running before taking a screenshot (seconds)

	//Engine Properties
	//Note: Higher fps will result in faster simulation speed (144 FPS is currently the "sweetspot")

	int g_fps_limit											 = 144;				// Monitor refreshrate: (x < 0), No limit: (x = 0)

	//Fluid Simulation Properties
	const unsigned int c_RESOLUTION							 = 256;				// The amount of cells that the velocityfield will contain the fluid. Note: The visual resolution is 1.333333 times larger than this resolution.
	//Experimental
	const float c_precision									 = 1.0f / 144.0f;	// The time step for each new iteration
	const bool  c_precision_bound							 = false;			// If the simulator should account for time lost by lag

	
public:
	struct Time
	{
	private:
		double deltaTime = 0;
		double fixedDeltaTime = 0;
		double pc_time = 0;

		static Time& Instance()
		{
			static Time instance;
			return instance;
		}
	public:
		static double DeltaTime(double value)
		{
			return Instance().deltaTime = value;
		}

		static double DeltaTime()
		{
			return Instance().deltaTime;
		}

		static double FixedDeltaTime(double value)
		{
			return Instance().fixedDeltaTime = value;
		}

		static double FixedDeltaTime()
		{
			return Instance().fixedDeltaTime;
		}

		static double PcTime(double value)
		{
			return Instance().pc_time = value;
		}

		static double PcTime()
		{
			return Instance().pc_time;
		}
	};
	//Time time;

	Engine();

	~Engine()
	{
		glfwTerminate();

		delete m_fluid;

		m_fluid = NULL;
		std::cout << "DESTROYED::ENGINE" << std::endl;
	}

	/*Time Time()
	{
		return Instance().s_time;
	}*/

	void Run();
	void update();
	void physicsUpdate();
	static void Pause();


private:
	void saveImage(const char* path, GLFWwindow* window);
	void saveResults();
	void calculateDeltatime();
	void calculateSleeptime();
	void calculateFPS();
	void constrainMouse(GLFWwindow* window, double xPos, double yPos);

	static void FRAMEBUFFER_RESIZE_CALLBACK(GLFWwindow* window, int width, int height);			//Is called when the window is resized
	static void MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos);					//Is called when mouse is being used
	static void KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods);	//Is called when keyboard events occur
	static void WINDOW_ICONIFY_CALLBACK(GLFWwindow* window, int iconified);						//Is called when the window is "minimized"
	static void WINDOW_FOCUS_CALLBACK(GLFWwindow* window, int focused);							//Is called when the window is "minimized"
	void IO_EVENTS(GLFWwindow* window);															//Handles input and output events
};

