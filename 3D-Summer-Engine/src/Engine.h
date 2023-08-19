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
#include "object_includes.h"
#include "FluidSimulation.h"
#include "Compute.h"
#include "Time.h"


class Engine
{
private:
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

	bool  g_save_result										 = false;			//If set to TRUE a screenshot will be saved after the desired time below
	const double g_save_result_time							 = 20.0f;			//How long the simulation has to be running before taking a screenshot (seconds)

	int g_fps_limit											 = 144;				//Modes: (x < 0) = Monitor refreshrate, (x == 0) = No limit
	double g_fixedDeltaTime									 = 1.0f / 60.0f;	// The time step for each new iteration

	//Fluid Simulation Properties
	const unsigned int c_RESOLUTION							 = 256;				// The amount of cells that the velocityfield will contain the fluid. Note: The visual resolution is 1.333333 times larger than this resolution.


public:
	static struct Mouse
	{
		double lastX = 0;
		double lastY = 0;
		bool leftMouseDown = 0;
		bool rightMouseDown = 0;
		bool firstMouseEnter = false;
	} g_mouse;

	struct Performance
	{
	private:
		double deltaTime = 0;
		double sleepTime = 0;
		double fps = 0;

		static Performance& Instance()
		{
			static Performance instance;
			return instance;
		}

	public:

		static double SetDeltaTime(double _deltaTime)
		{
			return Instance().deltaTime = _deltaTime;
		}

		static int FPS()
		{
			return Instance().fps;
		}

		static int FPS(int frames)
		{
			if (Instance().deltaTime == 0) return 0;
			return (int) (frames / Instance().deltaTime);
		}

		static int CalculateFPS(int frames, double _deltaTime)
		{
			if (_deltaTime == 0) return 0;
			return (int) roundf(frames/_deltaTime);
		}

		static double SleepTime(unsigned int maxFPS)
		{
			return Instance().SleepTime(maxFPS, Instance().deltaTime);
		}

		static double SleepTime(unsigned int maxFPS, double deltaTime)
		{
			if (maxFPS == 0) return 0;
			double dtMax = 1.0f / maxFPS;
			double renderTime = deltaTime - Instance().sleepTime;
			double time = (dtMax - renderTime);

			if (time < 0)
			{
				time = 0;
			}

			Instance().sleepTime = time;
			return time; //ms
		}
	};

	Engine();

	~Engine()
	{
		glfwTerminate();

		delete m_fluid;

		m_fluid = NULL;
		std::cout << "DESTROYED::ENGINE" << std::endl;
	}

	void Run();
	void render();
	void update(double deltaTime);
	void fixedUpdate(double deltaTime);
	static void Pause();

private:
	void saveImage(const char* path, GLFWwindow* window);
	void saveResults();
	void constrainMouse(GLFWwindow* window, double xPos, double yPos);

	static void FRAMEBUFFER_RESIZE_CALLBACK(GLFWwindow* window, int width, int height);
	static void MOUSE_CALLBACK(GLFWwindow* window, double xPos, double yPos);
	static void KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void WINDOW_ICONIFY_CALLBACK(GLFWwindow* window, int iconified);
	static void WINDOW_FOCUS_CALLBACK(GLFWwindow* window, int focused);
	void IO_EVENTS(GLFWwindow* window);
};

