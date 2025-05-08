#pragma once
#include "Core/Core.h"
#include "Logging/Log.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "InspectorPanel.h"
#include "ContentPanel.h"
#include "ConsolePanel.h"

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity ent);

	private:
		void DrawEntityNode(Entity ent);

		//void RightClickMenu();
		void RightClickMenu();

	private:
		Ref<Scene> m_Context;
		Ref<InspectorPanel> m_InspectorPanel;
		ContentPanel* m_ContentPanel;
		Entity m_SelectionContext;
	};
}