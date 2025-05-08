#include "Engine_PCH.h"
#include "EditorLayer.h"
#include "ImGui/ImGuiLibrary.h"
#include "ImGuizmo.h"
#include "Math/Math.h"

#include "Utils/PlatformUtils.h"
#include "Scene/SceneSerialiser.h"
#include <Physics/Raycast.h>
#include "Project.h"

namespace Engine
{

	EditorLayer::EditorLayer()
		: Layer("Editor Layer")
	{

	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

		FramebufferSpecification frameBufferSpec;
		frameBufferSpec.Attatchments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		frameBufferSpec.Width = 1280;
		frameBufferSpec.Height = 720;

		//m_Framebuffer = CreateRef<Framebuffer>(frameBufferSpec);
		m_Framebuffer = Framebuffer::Create(frameBufferSpec);
			
		m_Project = CreateRef<Project>();
		//m_GUIManager = CreateRef<GUIManager>();
		//m_GUIManager->Init({ m_ActiveScene->GetViewportWidth(), m_ActiveScene->GetViewportHeight() });

		//Project::m_Sound = CreateRef<SoundSystem>();
		//SoundSystem::PlaySound("src/Audio/Defaults/startup.mp3");

		//m_SoundSystem = CreateRef<SoundSystem>();
		m_SoundSystem = Application::Get().m_SoundSystem;

		m_SoundSystem->PlaySound("src/Audio/Defaults/startup.mp3");


		NewScene();

		//set the current code project to mono
		MonoManager::CurrProj = Project::currCodeProj.filename().string();
		//initlialise mono to load the dlls for scripting
		MonoManager::CompileScripts();
		MonoManager::Initialize();
		MonoManager::SceneContext = m_ActiveScene.get();


		m_ShaderLibrary = CreateRef<ShaderLibrary>();
		m_ShaderLibrary->Load("FlatColour", "assets/shaders/FlatColourShader.glsl");


		//creating gui layers
		m_ConsolePanel = new ConsolePanel();
		m_ConsolePanel->SetContext(m_ActiveScene);
		m_Profiler = new ProfilingPanel();

		//create a console buffer and redirect std to this buffer
		m_ConsoleBuffer = new ConsoleBuffer();
		std::cout.rdbuf(m_ConsoleBuffer);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{


		//resize framebuffer if necerssary
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		Renderer2D::SetClearColor({ 0.2, 0.2, 0.2, 1.0f });
		Renderer2D::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		/*
		//m_Renderer3D->PreProcessing();


		//these functions for renderer 2D are handled above
		m_Renderer2D->Clear();
		m_Renderer2D->SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		m_Renderer2D->ResetStats();


		m_Renderer2D->BeginScene(m_EditorCamera);


		//auto m_shader = m_ShaderLibrary->Get("FlatColour");
		//m_shader->Bind();
		//m_shader->UploadUniformFloat4("u_Color", { 0.75f, 0.5f, 0.0f, 1.0f });
		//m_shader->SetMat4("u_ViewProjection", m_EditorCamera.GetViewProjection());
		//m_shader->SetMat4("u_Transform", glm::mat4(1.0f));
		//
		//m_Renderer3D->RenderTriangle();


		for (float c = -5; c < 5; c++)
		{
			for (float r = -5; r < 5; r++)
			{
				m_Renderer2D->DrawQuad(glm::vec3(c, r, 0.0f), m_size, m_color);
			}
		}

		m_Renderer2D->EndScene();

		m_Framebuffer->Unbind();
		*/


		//m_EditorCamera.OnUpdate(ts);

		//update children
		/*
		auto view = m_ActiveScene->GetAllEntitiesWith<RelationshipComponent, TransformComponent>();
		for (auto e : view)
		{
			Entity otherEnt{ e, m_ActiveScene.get() };
			otherEnt.UpdateToParent();

			/*
			try
			{
				// Check if the entity has a parent and if the parent is valid
				if (otherEnt.HasComponent<RelationshipComponent>() && otherEnt.GetComponent<RelationshipComponent>().parent != NULL)
				{
					Entity parent = otherEnt.GetParent();
					if ((uint32_t)parent != entt::null)
					{
						if (!parent.HasComponent<RelationshipComponent>())
							continue;

						const auto& pr = parent.GetComponent<RelationshipComponent>();
						const auto& pt = parent.GetComponent<TransformComponent>();


						//iterative loop for all hierarichal parents
						//glm::mat4 transform = pt.GetTransform();
						//glm::vec3 translation, rotation, scale;
						//Math::DecomposeTransform(transform, translation, rotation, scale);
						//
						////otherEnt.GetComponent<TransformComponent>().Translation += parent.GetComponent<TransformComponent>().Translation;
						////
						//// Get the parent's global transformation matrix
						//while ((uint32_t)parent.GetParent() != entt::null)
						//{
						//	transform = parent.GetComponent<TransformComponent>().GetTransform() + transform;
						//	if ((uint32_t)parent == entt::null)
						//	{
						//		//we found the master parent, leave the loop
						//		break;
						//	}
						//	parent = parent.GetParent();
						//}
						//
						//Math::DecomposeTransform(transform, translation, rotation, scale);

						 // Compute the child's local rotation relative to the parent

						otherEnt.GetComponent<TransformComponent>().WorldPos	= pt.Translation + pt.WorldPos;
						otherEnt.GetComponent<TransformComponent>().WorldRot	= pt.Rotation + pt.WorldRot;
						otherEnt.GetComponent<TransformComponent>().WorldScale	= pt.Scale + pt.WorldScale - glm::vec3(1.0);

						//if (parent.HasComponent<RelationshipComponent>() && pr.parent == NULL)
						//{
						//	otherEnt.GetComponent<TransformComponent>().WorldPos	= pt.Translation;
						//	otherEnt.GetComponent<TransformComponent>().WorldRot	= pt.Rotation;
						//	otherEnt.GetComponent<TransformComponent>().WorldScale	= pt.Scale - glm::vec3(1.0);
						//}
						//else if(parent.HasComponent<RelationshipComponent>() && pr.parent != NULL)
						//{
						//	otherEnt.GetComponent<TransformComponent>().WorldPos	=	pt.Translation + pt.WorldPos;
						//	otherEnt.GetComponent<TransformComponent>().WorldRot	=	pt.Rotation + pt.WorldRot;
						//	otherEnt.GetComponent<TransformComponent>().WorldScale	=	pt.Scale + pt.WorldScale - glm::vec3(1.0);
						//}
						throw 505;
					}
				}
			}
			catch (...)
			{
				//something went wrong with components
			}

		}
		*/

		//update children
		{

			auto ents = m_ActiveScene->GetAllEntitiesWith<RelationshipComponent, TransformComponent>();
			for (auto e : ents)
			{
				Entity otherEnt{ e, m_ActiveScene.get() };
				otherEnt.UpdateToParent();
			}

		}


		switch (m_SceneState)
		{
		case SceneState::Edit:
			m_EditorCamera.OnUpdate(ts);
			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;

		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}

		//get the entity under the mouse position within the viewport
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportWidth = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = m_ViewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}


		m_Framebuffer->Unbind();

		auto stats = Renderer2D::GetStats();
		//LOG_INFO("Draw Calls: {0}", stats.DrawCalls);

	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		ImGUILibrary::CreateDockspace(dockspaceOpen, "Dockspace Demo");

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit", "Ctrl+Q"))
				{
					dockspaceOpen = false;
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Data"))
			{
				//if (m_Profiler == nullptr || m_Profiler->show == false)
				//{
				//	if (ImGui::MenuItem("Show Profiler"))
				//	{
				//		if (m_Profiler == nullptr)
				//			m_Profiler = new ProfilingPanel();
				//
				//		m_Profiler->show = true;
				//
				//	}
				//}
				//else if(m_Profiler != nullptr && m_Profiler->show == true)
				//{
				//	if (ImGui::MenuItem("Stop Profiler"))
				//	{
				//		m_Profiler->show = false;
				//	}
				//}
				if (ImGui::MenuItem(m_Profiler->show == true ? "Stop Profiler" : "Show Profiler"))
				{
					m_Profiler->show = !m_Profiler->show;
					if (m_Profiler->show == true)
					{
						m_Profiler->StartRecording();
					}
					else 
					{
						m_Profiler->StopRecording();
					}
				}
				
				if (ImGui::MenuItem("Recompile Scripts"))
				{
					MonoManager::CompileScripts();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();

		}

		//not needed because of scene hierarchy panel.cpp
		////CREATING DUMMY FOR SCENE HIERARCHY
		//ImGui::Begin("Scene Hierarchy");
		//ImGui::Text("Hello World");
		//ImGui::End();

	#pragma region ImGuiDisplay

		ImGui::PushFont(UI::Fonts["montserrat"]);
		//render the scene hierarchy panel
		m_SceneHierarchyPanel.OnImGuiRender();


		//create new viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport");

	#pragma region Gizmo toolbar
		float toolbarHeight = 30.0f;

		//button styles
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 5.0f, 0.0f });
		int* styles = new int(3); //how many styles to pop later

		//set window size for the toolbar (full width and specified height)
		ImGui::BeginChild("Toolbar", ImVec2(ImGui::GetContentRegionAvail().x, toolbarHeight), true);
		ImVec2 buttonSize = { 60, toolbarHeight };

		if (ImGui::Button("Move", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Rotate", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Scale", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
		}

		ImGui::PopStyleVar(*styles);
		delete styles;

		//scene play state buttons
		ImGui::NextColumn();
		ImGui::SameLine();

		// Calculate the position for the "Play" button
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float buttonsWidth = buttonSize.x * 3; // Width of Move, Rotate, Scale buttons
		//float playButtonPosX = (availableWidth - buttonsWidth - buttonSize.x) / 2 + buttonsWidth; //dont minus the buttons width, keep the offset of those buttons
		//float playButtonPosX = (availableWidth - buttonSize.x) / 2 + buttonsWidth - buttonSize.x; //do this if a second button, e.g. a play and a stop
		float playButtonPosX = (availableWidth - buttonSize.x) / 2 + buttonsWidth;

		ImGui::SetCursorPosX(playButtonPosX); // sets the X position of the cursor within the current window or child window. Wont effect next child

		char* buttonName = (m_SceneState == SceneState::Edit) ? "Play" : "Stop";

		ImGui::PushFont(UI::Fonts["righteous"]); //fonts are compiled when in ImGuilayer.OnAttach()

		if (ImGui::Button(buttonName, buttonSize))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
				//m_SceneState = SceneState::Play;
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
				//m_SceneState = SceneState::Edit;
			}
		}
		//this is below the button because it checks whether the last item (in this case, the button) is being hovered over
		if (ImGui::IsItemHovered()) //show a tooltip when this button above is hovered over
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted("Switch play mode: Ctrl + P");
			ImGui::EndTooltip();
		}
		ImGui::PopFont();



		//Will enable two buttons, one for playing and one for stopping instead of a swapping button
		/*
		ImGui::SameLine();

		ImGui::BeginDisabled((m_SceneState == SceneState::Edit) ? true : false);
		if (ImGui::Button("Stop", buttonSize))
		{
			if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}
		ImGui::SameLine();
		ImGui::BeginDisabled((m_SceneState == SceneState::Play) ? true : false);
		if (ImGui::Button("Play", buttonSize))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
		}
		ImGui::EndDisabled();
		*/

		//add more buttons here^

		ImGui::EndChild();

		ImGui::Begin("Console");
		m_ConsoleBuffer->sync();
		m_ConsolePanel->SetContext(m_ActiveScene);
		m_ConsolePanel->Render(*m_ConsoleBuffer);
		ImGui::End();

		if (m_Profiler->show)
		{
			m_Profiler->Render();
		}


	#pragma endregion

		// --- Set Up Viewport Bounds Correctly ---
		// After ending the toolbar, the current cursor position is the start of the remaining viewport.
		ImVec2 viewportOffset = ImGui::GetCursorScreenPos();

		//auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		//auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		//auto viewportOffset = ImGui::GetWindowPos();

		//m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		//m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewPortFocused = ImGui::IsWindowFocused();
		m_ViewPortHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused && !m_ViewPortHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColourAttachmentRendererID();
		ImGui::Image((ImTextureID)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });

		// Define viewport bounds based on the new offset and available size.
		m_ViewportBounds[0] = { viewportOffset.x, viewportOffset.y };
		m_ViewportBounds[1] = { viewportOffset.x + viewportPanelSize.x, viewportOffset.y + viewportPanelSize.y };


#pragma region Gizmos

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_ActiveScene->isRunning == false && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(
				viewportOffset.x,
				viewportOffset.y,
				viewportPanelSize.x,
				viewportPanelSize.y);

			//ImGuizmo::SetRect(
			//	m_ViewportBounds[0].x, m_ViewportBounds[0].y, 
			//	m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
			//	m_ViewportBounds[1].y - m_ViewportBounds[0].y);
			//\/ this became this ^ after implementing openGL framebuffer utils
			//float windowWidth = (float)ImGui::GetWindowWidth();
			//float windowHeight = (float)ImGui::GetWindowHeight();
			//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);


			//editor camera
			const glm::mat4& camProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto& entityTransform = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = entityTransform.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapVal = 0.5f;
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			{
				snapVal = 45.0f;
			}

			float snapValues[3] = { snapVal, snapVal, snapVal };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(camProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);


			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 WorldTranslation = translation - entityTransform.WorldPos;
				glm::vec3 WorldScale = scale - entityTransform.WorldScale;
				//entityTransform.Translation = translation;
				entityTransform.Translation = translation - entityTransform.WorldPos;
				entityTransform.Scale = scale - entityTransform.WorldScale;

				glm::vec3 deltaRotation = rotation - entityTransform.WorldRot;
				entityTransform.Rotation = deltaRotation;

			}

		}

#pragma endregion

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopFont();

		//end dockspace
		ImGui::End();
#pragma endregion

	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));

		////is this event a window resize event? if so, run this scripts onwindowresize function
		//EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowResizeEvent>(
		//	std::bind(&EditorLayer::OnWindowResize, this, std::placeholders::_1)
		//);
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewPortHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}

		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent e)
	{
		if (e.GetRepeatCount() > 0)
		{
			return false;
		}

		bool control = Input::IsKeyPressed(Key::LeftControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift);

		switch (e.GetKeyCode())
		{

		case Key::N:
			if (control)
				NewScene();
			break;

		case Key::S:
			if (control && !shift)
				SaveScene();
			if (control && shift)
				SaveSceneAs();
			break;

		case Key::O:
			if (control)
				OpenScene();
			break;

		case Key::Q:
			if (control)
				Application::Get().Close();
			break;

		case Key::D:
			if (control)
				OnDuplicateEntity();
			break;

		case Key::Delete:
			OnDeleteEntity();
			break;

		case Key::P:
			if (control)
			{
				//(m_SceneState == SceneState::Edit) ? OnScenePlay() : OnSceneStop();;
				if (m_SceneState == SceneState::Edit)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
			break;

		case Key::W:
			if (!ImGuizmo::IsUsing() && m_ViewPortHovered)
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

		case Key::E:
			if (!ImGuizmo::IsUsing() && m_ViewPortHovered)
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;

		case Key::R:
			if (!ImGuizmo::IsUsing() && m_ViewPortHovered)
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;


		}
	}

	void EditorLayer::NewScene()
	{
		//m_EditorScene = CreateRef<Scene>("New Scene", true);
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->m_IsEditorScene = true;
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);



		//create basic objects
		Entity cam = m_EditorScene->CreateEntity("Main Camera");
		cam.AddComponent<CameraComponent>();
	}

	void EditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("Scene(*.BEngine)\0*.BEngine\0");
		if (!path.empty())
		{
			OpenScene((std::filesystem::path)path);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		if (path.extension().string() != ".BEngine")
		{
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}
		m_EditorSceneFilePath = path;

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerialiser serializer(newScene);
		if (serializer.Deserialise(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
		}
		m_ActiveScene = m_EditorScene;
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorSceneFilePath.empty())
		{
			SerialiseScene(m_EditorScene, m_EditorSceneFilePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		//std::string path = PlatformUtils::ShowFileOpen();
		std::string path = FileDialogs::SaveFile("Scene(*.BEngine)\0*.BEngine\0");
		if (!path.empty())
		{
			SerialiseScene(m_EditorScene, path);
		}
		m_EditorSceneFilePath = path;
	}

	void EditorLayer::SerialiseScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerialiser serializer(scene);
		serializer.Serialise(path.string());
	}


	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;
		m_RuntimeScene = CreateRef<Scene>();
		m_RuntimeScene->isRunning = true;

		//start a physics layer
		m_PhysicsLayer = CreateRef<PhysicsSystem>();

		Scene::CopyTo(m_EditorScene, m_RuntimeScene);
		//m_SoundSystem->StopSystem();
		m_RuntimeScene->RuntimeSound = m_SoundSystem;

		m_ActiveScene = m_RuntimeScene;

		m_RuntimeScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}
	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_RuntimeScene->OnRuntimeStop();

		//unload the runtime scene
		m_RuntimeScene = nullptr;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_ActiveScene->isRunning = false;

		//reset the physics layer
		m_PhysicsLayer.reset();

		MonoManager::SceneContext = m_ActiveScene.get();
	}


	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Entity Duplication permitted in Edit Mode ONLY");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
		}
	}

	void EditorLayer::OnDeleteEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Entity Deletion permitted in Edit Mode ONLY");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DestroyEntity(m_SceneHierarchyPanel.GetSelectedEntity());
			m_SceneHierarchyPanel.SetSelectedEntity({}); // Must clear to avoid a crash
		}
	}


	//bool EditorLayer::OnWindowResize(WindowResizeEvent& e)
	//{
	//	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	//	{
	//		return false;
	//	}
	//
	//	LOG_INFO("Window Size: {0}, {1}", e.GetWidth(), e.GetHeight());
	//	m_Renderer3D->UpdateViewportSize(e.GetWidth(), e.GetHeight());
	//
	//	return false;
	//}

}
