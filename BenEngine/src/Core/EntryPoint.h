#pragma once
#include <Core/Core.h>

//defines the global engine application function - function defined in BenEngine.cpp
extern Engine::Application* Engine::CreateApplication();

int main(int arc, char** argv)
{
	Engine::Log::Init();

	printf("Ben Engine\n");
	auto app = Engine::CreateApplication();

	app->Run();

	delete app;
}

//force GPU usage
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablment = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
