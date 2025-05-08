#pragma once
#include "Engine.h"
#include "Editor/Panels/SceneHierarchyPanel.h"
#include <filesystem>

#include "Editor/Project.h"
#include "Physics/PhysicsSystem.h"

#include "Audio/SoundSystem.h"
#include "Panels/ProfilingPanel.h"


namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		void OnEvent(Event& e) override;

	private:
		bool OnMouseButtonPressed(MouseButtonPressedEvent e); //to check if the user is clicking on the editor layer
		bool OnKeyPressed(KeyPressedEvent e);

		void OnScenePlay();
		void OnSceneStop();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SerialiseScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnDuplicateEntity();
		void OnDeleteEntity();

	private:
		//bool OnWindowResize(WindowResizeEvent& e); //handle changing the layer properties if window is resized
		EditorCamera m_EditorCamera;
		Ref<Framebuffer> m_Framebuffer;
		Ref<ShaderLibrary> m_ShaderLibrary;
		Ref<Project> m_Project;
		Ref<SoundSystem> m_SoundSystem;

		//Ref<GUIManager> m_GUIManager;

	public:
		Ref<Scene> m_ActiveScene;

		ProfilingPanel* m_Profiler;
		ConsolePanel* m_ConsolePanel;
		//manages console ouput
		ConsoleBuffer* m_ConsoleBuffer;



	private:
		Ref<PhysicsSystem> m_PhysicsLayer;

		//Ref<MonoManager> m_Mono;

		Ref<Scene> m_EditorScene, m_RuntimeScene;
		std::filesystem::path m_EditorSceneFilePath;

		int m_GizmoType = -1;

		//these renderers are no longer needed because active scene can call components from entities that can render
		//Ref<Renderer3D> m_Renderer3D;
		//Ref<Renderer2D> m_Renderer2D;

		bool m_ViewPortFocused = false;
		bool m_ViewPortHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		Entity m_HoveredEntity;

		SceneHierarchyPanel m_SceneHierarchyPanel;

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Pause = 2,
			Simulate = 3
		};

		SceneState m_SceneState = SceneState::Edit;

		//we now have an inspector panel to handle this
		////for testing the inspector
		//Entity m_quadEntity;
		//Entity m_CameraEntity;
		//glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		//glm::vec4 m_colour = { 0.75f, 0.5f, 0.0f, 1.0f };
		//glm::vec2 m_size = { 0.5f, 0.5f };
	};
}

