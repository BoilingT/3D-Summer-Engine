#include "Engine.h"

#ifdef _DEBUG
int main() {
#else
int WinMain() {
#endif
	Engine engine;
	engine.Run();
}