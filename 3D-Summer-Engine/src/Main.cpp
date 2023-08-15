//#include "FluidSimulation.h"
#include "Engine.h"

#ifdef _DEBUG
int main()
{
#else
int WinMain()
{
#endif

	//FluidSimulation fluidSimulation;
	//fluidSimulation.Run();

	Engine engine;
	engine.Run();
}