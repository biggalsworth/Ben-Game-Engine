#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	
	class ImGUILibrary
	{
	public:
		static void CreateDockspace(bool& dockspaceOpen, char* dockspaceName)
		{
			static bool opt_fullscreen = true;
			static bool opt_padding = false;

			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			//using the no docking flag so the parent window is not dockable into, two docking targets within eachother would be confusing
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();

				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			}
			else
			{
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			}

			//Using ImGuiDockNodeFlags_PassthruCentralNode, the dockspace() will render our background and
			//handle the pass-thru hole, so we ask Begin to no render a background
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			//we will proceed even if begin() returns false (aka window is collapsed)
			//this is becasue we want to keep our dockspace active, if it is inactive 
			//then all active windows docked into it will lose their parent and become undocked
			//we cannot preserve the docking relationship between active windows and inactive docking
			//otherwise any change of dockspace/settings would lead to windows being stuck in limbo and never be visible.

			if (!opt_padding)
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGui::Begin(dockspaceName, &dockspaceOpen, window_flags);

			if (!opt_padding)
				ImGui::PopStyleVar();
			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

		}

		//displays as many menu items as is given, then when a menu item is clicked the UI function is forwarded to be called
		template<typename UIFunction, typename... Args>
		static void DrawMenuItem(const std::string& label, UIFunction uiFunc, Args&&... args)
		{
			if (ImGui::MenuItem(label.c_str()))
			{
				uiFunc(std::forward<Args>(args)...);
			}
		}

		//create a button that is centered within the available context
		template<typename UIFunction>
		static void DrawCentredButton(const std::string& label, float percentageWidth, UIFunction uiFunction)
		{
			ImVec2 regionAvailable = ImGui::GetContentRegionAvail();
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f; // Line height for the button

			// make the button take almost all the available space while leaving some margin
			float buttonWidth = regionAvailable.x * percentageWidth; // Use X% of available width
			float buttonXPos = (regionAvailable.x - buttonWidth) * 0.5f; // centre button

			ImGui::Columns(2, NULL, false); // Create two columns
			ImGui::SetColumnWidth(0, (regionAvailable.x - buttonWidth) * 0.5f); // set the width of the first column
			ImGui::NextColumn(); // move to second column where the button will be

			if (ImGui::Button(label.c_str(), ImVec2{ buttonWidth, lineHeight }))
			{
				uiFunction();
			}

			ImGui::NextColumn();	// move to third column (extra space)
			ImGui::Columns(1);		// reset to one column
		}

		//renders a collapsible UI for specific components of an entity
		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_AllowItemOverlap |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_FramePadding;

			if (entity.HasComponent<T>())
			{
				auto& component = entity.GetComponent<T>();
				ImVec2 regionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

				ImGui::Separator();

				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				ImGui::PopStyleVar();
				ImGui::SameLine(regionAvailable.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				bool removeComponent = false;
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component") && name != "Script Manager")
					{
						removeComponent = true;
					}

					if (ImGui::MenuItem("Copy Component"))
					{
						entity.GetScene()->CopyComponent<T>(entity);
					}

					bool isDisabled = !entity.GetScene()->HasCopiedComponent();

					ImGui::BeginDisabled(isDisabled);
					if (ImGui::MenuItem("Paste Component"))
					{
						entity.GetScene()->PasteComponent(entity);
					}
					ImGui::EndDisabled();


					ImGui::EndPopup();
				}

				if (open)
				{
					uiFunction(component);
					ImGui::TreePop();
				}

				if (removeComponent)
				{
					entity.RemoveComponent<T>();
				}
			}
		}

		//renders a UI Widget for editing a vec3 variable by providing 3 draggable input fields
		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			//creates a style for this section
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			ImGui::Columns(1);

			ImGui::PopID();
		}
		
		
		static void DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			//creates a style for this section
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar();
			ImGui::Columns(1);

			ImGui::PopID();
		}

		static bool Warning(std::string warningMessage, bool* opened)
		{
			bool result = false;

			ImGui::OpenPopup("WarningPopup");
			
			//modal is a blocking operation, the user cannot continue until they have interacted with this
			if (ImGui::BeginPopupModal("WarningPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(warningMessage.c_str());
			
				ImGui::Text("Are you sure you want to proceed?");
			
				ImGui::Separator();
			
				if (ImGui::Button("Yes", ImVec2(100, 0)))
				{
					result = true;
					*opened = false;
					//ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(100, 0)))
				{
					result = false;
					*opened = false;
					//ImGui::CloseCurrentPopup();
				}
			
				ImGui::EndPopup();
			}

			return result;
		}

		static void DrawSearchWindow(std::string label, std::string* outputString, std::string extension = "")
		{

			static char searchBuffer[128]; // Buffer for storing search text
			static std::vector<std::filesystem::path>* fileList = new std::vector<std::filesystem::path>(); // file names


			if (ImGui::Button(label.c_str()))
			{
				ImGui::OpenPopup(label.c_str());
				searchBuffer[0] = '\0'; // Empty the buffer
				//currentDir = std::filesystem::path("assets");
				fileList->clear();
			}
			if (ImGui::BeginPopup(label.c_str()))
			{
				ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

				ImGui::SameLine();

				if (ImGui::Button("Search") || ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					if (searchBuffer[0] == '\0')
					{
						fileList->clear();
					}
					else
					{
						std::cout << searchBuffer << std::endl;
						fileList = Project::FindFileInSubfolders(Project::currCodeProj, searchBuffer);
						if (fileList->size() == 0)
							fileList = Project::FindFileInSubfolders(std::filesystem::path("assets"), searchBuffer);
					}


				}

				if (fileList->size() > 0)
				{
					for (int i = 0; i < fileList->size(); i++)
					{
						std::filesystem::path& it = (*fileList)[i];  // Get the current element in the list
						if (extension.empty() || it.filename().extension() == extension)
						{
							if (ImGui::MenuItem(it.filename().string().c_str()))  // Corrected MenuItem function call
							{
								*outputString = it.string();
								std::cout << "File:" << it.string() << std::endl;
							}
						}
					}
				}
				else
				{

					for (auto& it : std::filesystem::directory_iterator(std::filesystem::current_path()))
					{
						const auto& path = it.path(); // the path of the current iterator
						std::filesystem::relative(path, std::filesystem::path("assets"));
						auto relativePathString = path.string(); //string of the path we are looking at

						if (extension.empty() || path.extension() == extension)
						{
							if (ImGui::MenuItem(relativePathString.c_str()))
							{
								*outputString = path.string();
								std::cout << relativePathString.c_str() << std::endl;
							}
						}
					}



					for (auto& it : std::filesystem::recursive_directory_iterator(Project::currCodeProj / "Assets"))
					{
						const auto& path = it.path(); // the path of the current iterator
						//std::filesystem::relative(path, std::filesystem::path("Assets"));
						auto relativePathString = path.string(); //string of the path we are looking at

						if (extension.empty() || path.extension() == extension)
						{
							if (ImGui::MenuItem(path.filename().string().c_str()))
							{
								*outputString = path.string();
								std::cout << relativePathString.c_str() << std::endl;

							}
						}
					}
				}

				ImGui::EndPopup();

			}
		}
	};
}