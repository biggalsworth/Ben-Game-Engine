#pragma once
#include "Core/Core.h"
#include "Core/Timestep.h"
#include "Events/Event.h"

//creating the base class for a layer
namespace Engine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}; //when a layer is attached to the application layer stack
		virtual void OnDetach() {}; //when a layer is detattched from the application (removed from layer stack)
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(Event& event) {};

		inline const std::string& GetName() const { return m_DebugName; }
	
	private:
		std::string m_DebugName;
	};
}