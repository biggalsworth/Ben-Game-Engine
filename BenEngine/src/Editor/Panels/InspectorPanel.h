#pragma once
//#include "Core/Core.h"
#include "Scene/Entity.h"

namespace Engine
{
	class InspectorPanel
	{
	public:
		InspectorPanel() {};
		~InspectorPanel() = default;

		void DrawComponents(Entity ent);

		void DrawAddComponent(Entity ent);

	private:
		template<typename T>
		void DrawAddComponentMenuItem(Entity ent, const char* label);
		void DrawAddScript(Entity ent);
	};
}