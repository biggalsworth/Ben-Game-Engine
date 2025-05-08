#pragma once
#include "Core.h"
#include "Window.h"
#include "Core/Timestep.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"
#include "imgui.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Audio/SoundSystem.h"

int main(int argc, char** argv);

namespace UI
{
	using namespace Engine;
	//Loading fonts
	//if no font is loaded, ImGui will load the default font
	extern std::unordered_map<std::string, ImFont*> Fonts;

	void SetupFonts();
}

// Custom buffer to capture output
class ConsoleBuffer : public std::stringbuf {
public:
	std::vector<std::string> lines;

	int sync() override 
	{
		lines.emplace_back(str());
		str("");
		return 0;
	}
};

namespace Engine
{

	class Application
	{
	public:
		//the creator for Application class
		Application(const std::string& name = "BenEngine");
		~Application();

		void OnEvent(Event& e);

		//layer handling
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		//get the main (current) window
		inline Window& GetWindow() { return *m_Window; }

		//the function that will close the application
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		//get the current application
		static Application& Get() { return *s_Instance; }

		//reference to a sound system
		Ref<SoundSystem> m_SoundSystem;


	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		//the pointer to the main window
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		//variables to store while the application is running
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;

	private:
		//a reference to the application instance that is running
		static Application* s_Instance;
		//allows application to call main
		friend int ::main(int argc, char** argv);
	};
	Application* CreateApplication();
};