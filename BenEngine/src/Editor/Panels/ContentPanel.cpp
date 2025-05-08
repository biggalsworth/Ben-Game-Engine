#include "Engine_PCH.h"
#include "ContentPanel.h"
#include <imgui.h>
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

#include <imgui/imgui.h>
#include "ImGui/ImGuiLibrary.h"
#include <glm/gtc/type_ptr.hpp>
#include <Engine.h>

#include <GraphicsAPI/stb_image.h>


namespace Engine
{
    static const std::filesystem::path s_AssetsPath= "assets";
    ContentPanel::ContentPanel() : 
        m_CurrentDirectory(s_AssetsPath)
    {
    }

    void ContentPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
        Setup();
        //m_CurrentDirectory = s_AssetsPath;

    }

    void ContentPanel::Setup()
    {
        //m_CurrentDirectory = s_AssetsDirectory;

        //ThumbnailSize = new float(128.0f);
        //Padding = new float(16.0f);
        //ShowPhysicsColliders = false;
    } 

    void ContentPanel::ListDirectoryContents(const std::filesystem::path& directoryPath)
    {

        ImGui::BeginChild("Folder", ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, ImGui::GetContentRegionAvail().y), true);
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

        //list the folders
        for (auto& it : std::filesystem::directory_iterator(Project::currCodeProj / "Assets/"))
        {
            const auto& path = it.path(); // Path of the current iterator
            std::filesystem::relative(path, Project::currCodeProj);
            auto relativePathString = path.string(); // String of the path
        
            if (it.is_directory()) // If the iterator is a folder
            {
                bool opened = ImGui::TreeNodeEx(path.filename().string().c_str(), flags);


                if (ImGui::IsItemClicked())
                { 
                    m_SelectedPath = std::filesystem::absolute(path);
                }

                if (opened) // Create a dropdown
                {
                    for (auto& fileIt : std::filesystem::directory_iterator(path))
                    {
                        const auto& filePath = fileIt.path();
                        auto fileRelativePathString = filePath.filename().string();

                        if (fileIt.is_directory()) // Files within the folder
                        {
                            ListFolders(std::filesystem::absolute(path));
                        }
                    }
                    ImGui::TreePop(); // Close the dropdown
                }
            }
        }

        ImGui::Separator();
        ImGui::Text("Engine Files");
        for (auto& it : std::filesystem::directory_iterator(s_AssetsPath))
        {
            const auto& path = it.path(); // Path of the current iterator
            std::filesystem::relative(path, s_AssetsPath);
            auto relativePathString = path.string(); // String of the path
        
            if (it.is_directory()) // If the iterator is a folder
            {
                bool opened = ImGui::TreeNodeEx(("Engine/" + path.filename().string()).c_str(), flags);
                if (ImGui::IsItemClicked())
                {
                    m_SelectedPath = std::filesystem::absolute(path);
                }
        
                if (opened) // Create a dropdown
                {
                    for (auto& fileIt : std::filesystem::directory_iterator(path))
                    {
                        const auto& filePath = fileIt.path();
                        auto fileRelativePathString = filePath.filename().string();
        
                        if (fileIt.is_directory()) // Files within the folder
                        {
                            ListFolders(std::filesystem::absolute(path));
                        }
                    }
                    ImGui::TreePop(); // Close the dropdown
                }
            }
            else // For files in the current directory
            {
                if (ImGui::Selectable(path.filename().string().c_str()))
                {
                    system(("start " + path.string()).c_str()); // Opens with default program
                }
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();
        

        //list files in folder
        if (m_SelectedPath != "")
        {
            ImGui::BeginChild("Files", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);

            float panelWidth = ImGui::GetContentRegionAvail().x;
            float cellSize = 80 + 10; //first float is size, second is padding
            int columnCount = std::max(1, (int)(panelWidth / cellSize));
            if (ImGui::BeginTable("ContentBrowserTable", columnCount))
            {
                for (auto& it : std::filesystem::directory_iterator(m_SelectedPath))
                {
                    const auto& path = it.path(); // the path of the current iterator
                    //std::filesystem::relative(path, Project::currCodeProj);
                    auto relativePathString = path.string(); //string of the path we are looking at

                    ImGui::PushID(relativePathString.c_str());
                    if (!it.is_directory())// is the iterator a file
                    {
                        ImGui::TableNextColumn();

                        //if (ImGui::Button(relativePathString.c_str(), ImVec2(100, 100)))
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
                        //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2((ImGui::GetContentRegionAvail().x - 80.0f) / 2.0f, 0.0f));
                        GLuint texture = Project::LoadTextureFromFile("src/Extra/Icons/UI/file.png");
                        //if (ImGui::ImageButton(path.filename().string().c_str(), (ImTextureID)(intptr_t)texture, ImVec2(80.0f, 80.0f)))
                        if (ImGui::ImageButton(path.filename().string().c_str(), (ImTextureID)(intptr_t)texture, ImVec2(60.0f, 60.0f)))
                        {
                            system(("start " + relativePathString).c_str()); // Opens with the default program
                        }

                        //Allow the ability to drag files
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("FILE_PATH", relativePathString.c_str(), relativePathString.size() + 1);
                            ImGui::EndDragDropSource();
                        }

                        ImGuiIO io = ImGui::GetIO();
                        ImGui::PushFont(UI::Fonts["righteous-small"]); //fonts are compiled when in ImGuilayer.OnAttach()
                        ImGui::Text(path.filename().string().c_str());
                        ImGui::PopFont();
                        ImGui::PopStyleVar();
                        ImGui::PopStyleColor();
                    }
                    ImGui::PopID();
                }

                RightClickMenu();
                ImGui::EndTable();

            }
            ImGui::EndChild();
        }
    }

    void ContentPanel::ListFolders(const std::filesystem::path& directoryPath)
    {
        for (auto& it : std::filesystem::directory_iterator(directoryPath))
        {
            const auto& path = it.path(); // Path of the current iterator
            std::filesystem::relative(path, Project::currCodeProj);
            auto relativePathString = path.string(); // String of the path

            if (it.is_directory()) // If the iterator is a folder
            {
                bool opened = ImGui::TreeNodeEx(path.filename().string().c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
                if (ImGui::IsItemClicked())
                {
                    m_SelectedPath = std::filesystem::absolute(path);
                    LOG_INFO(std::filesystem::absolute(path).string());
                }

                if (opened) // Create a dropdown
                {
                    for (auto& fileIt : std::filesystem::directory_iterator(path))
                    {
                        const auto& filePath = fileIt.path();
                        auto fileRelativePathString = filePath.filename().string();

                        if (fileIt.is_directory()) // Files within the folder
                        {
                            ListFolders(std::filesystem::absolute(path));
                        }
                    }
                    ImGui::TreePop(); // Close the dropdown
                }
            }
        }
    }


    void ContentPanel::ShowSettingsPanel()
    {
        ImGui::Begin("Settings");
        //::Checkbox("Show physics colliders", &ShowPhysicsColliders);
        ImGui::End();
    }

    void ContentPanel::Render()
    {
        ListDirectoryContents("assets/");

        // Show settings panel
        //ShowSettingsPanel();
    }
    void ContentPanel::RightClickMenu()
    {
        if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
        {
            ImGUILibrary::DrawMenuItem("Create Folder", [this](Ref<Scene> m_Context) { FileSystem::CreateFolder(m_SelectedPath, "New Folder"); }, m_Context);

            ImGui::EndPopup();
        }
    }
}