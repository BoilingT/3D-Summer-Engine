//#include "FluidSimulation.h"
#ifdef _DEBUG
#define Main main
#else
#define Main WinMain
#endif

#include "Engine.h"

int Main()
{
	Engine engine;
	engine.Run();
}