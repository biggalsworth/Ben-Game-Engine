#pragma once
#include "Engine_PCH.h"
#include "Core.h"
#include "Events/Event.h"

namespace Engine
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		//create the default data for windowProps
		WindowProps(const std::string& title = "Engine",
			uint32_t width = 1280, uint32_t height = 720)
			:Title(title), Width(width), Height(height)
		{

		}
	};

	class Window
	{
	public:
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		//Window Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}

