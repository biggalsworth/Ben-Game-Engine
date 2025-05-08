#include "Engine_PCH.h"
#include "Application.h"

#include <GLFW/glfw3.h>

#include "Input.h"
#include "Renderer/2D/Renderer2D.h"

namespace UI
{
	//Loading fonts
	//if no font is loaded, ImGui will load the default font
	std::unordered_map<std::string, ImFont*> Fonts;

	void SetupFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
		ImFont* mainfont = io.Fonts->AddFontFromFileTTF("src/Editor/Fonts/montserrat/Montserrat-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		if (!mainfont)
		{
			printf("Failed to load font!\n");
		}
		else
		{
			try
			{
				UI::Fonts.insert({ "montserrat", mainfont });
				UI::Fonts.insert({ "righteous", io.Fonts->AddFontFromFileTTF("src/Editor/Fonts/righteous/Righteous-Regular.ttf", 18.5f, NULL, io.Fonts->GetGlyphRangesJapanese()) });
				UI::Fonts.insert({ "righteous-small", io.Fonts->AddFontFromFileTTF("src/Editor/Fonts/righteous/Righteous-Regular.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesJapanese()) });

			}
			catch(...)
			{
				LOG_ERROR("FONTS COULD NOT BE LOADED");
			}
		}

	}
}

namespace Engine 
{
#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)
	
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		//this is the screen instance
		s_Instance = this;
		//create the main window for the application
		m_Window = Window::Create(WindowProps(name));

		//create the sound system
		m_SoundSystem = CreateRef<SoundSystem>();

		//bind this window's event callback to this function OnEvent with 1 parameter
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	
		Input::Init(); // begin checking for inputs
		Input::SetEventCallback(BIND_EVENT_FN(Application::OnEvent)); //when application has an event, call input as well
	
		Renderer2D::Init();

		//add the imgui layer here so - only need the imgui layer if we have the editor layer
		//the imgui layer will handle setting up the configs and that is it, 
		//the other layers will use OnImGuiRender to actually create the interface using the imguilayer's configs
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Close()
	{
		//stop running
		m_Running = false;
	}

	//layer handling
	//add a layer to the stack
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}


	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e) 
	{
		//create an event dispatcher
		EventDispatcher dispatcher(e);
		//pass the event type we need
		//if the event type of e is the same as below, pass a binded event function
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if (e.Handled)
			{
				break;
			}
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				//update all the layers
				{
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}

				m_ImGuiLayer->Begin();
				{
					for (Layer* layer : m_LayerStack)
					{
						layer->OnImGuiRender();
					}
				}

				m_ImGuiLayer->End();

				Input::Update(); //update input for the window
			}

			m_Window->OnUpdate();
		}
	}



	//window interactions
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) 
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		return false;
	}
}