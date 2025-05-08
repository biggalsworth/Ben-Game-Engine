#pragma once
#include "Core/Core.h"
#include "Logging/Log.h"
#include "Scene/Scene.h"
#include <Engine.h>

namespace Engine
{
	class ConsolePanel
	{
	public:
		ConsolePanel();
		~ConsolePanel() = default;

		void SetContext(const Ref<Scene>& context);

		void Render(ConsoleBuffer& buffer);

	private:
		Ref<Scene> m_Context;

	};
}