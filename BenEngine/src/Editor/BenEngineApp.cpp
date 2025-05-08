#include "Engine_PCH.h"
#include "Engine.h"

#include "Core/EntryPoint.h"
#include "Editor/EditorLayer.h"
//#include "EditorLayer.h"

namespace Engine
{
	class BenEngineApp : public Application
	{
	public:
		BenEngineApp() : Application("Braveheart Engine Editor")
		{
			PushLayer(new EditorLayer());
		}
		~BenEngineApp() {}
	};

	Engine::Application* Engine::CreateApplication()
	{
		return new BenEngineApp();
	}
}