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
#include <Scene/SceneSerialiser.h>


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
        ListFolders(Project::currCodeProj / "Assets/");

        //for (auto& it : std::filesystem::directory_iterator(Project::currCodeProj / "Assets/"))
        //{
        //    const auto& path = it.path(); // Path of the current iterator
        //    std::filesystem::relative(path, Project::currCodeProj);
        //    auto relativePathString = path.string(); // String of the path
        //
        //    if (it.is_directory()) // If the iterator is a folder
        //    {
        //        bool opened = ImGui::TreeNodeEx(path.filename().string().c_str(), flags);
        //        if (ImGui::BeginDragDropTarget())
        //        {
        //            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
        //            {
        //                try
        //                {
        //                    std::filesystem::path dropped = static_cast<const char*>(payload->Data);
        //                    std::filesystem::path targetPath = path / dropped.filename(); // Destination path
        //                    std::filesystem::rename(dropped, targetPath); //rename its directory
        //
        //                }
        //                catch (...)
        //                {
        //
        //                }
        //            }
        //            ImGui::EndDragDropTarget();
        //        }
        //
        //        if (ImGui::IsItemClicked())
        //        { 
        //            m_SelectedPath = std::filesystem::absolute(path);
        //        }
        //
        //        if (opened) // Create a dropdown
        //        {
        //            for (auto& fileIt : std::filesystem::directory_iterator(path))
        //            {
        //                const auto& filePath = fileIt.path();
        //                auto fileRelativePathString = filePath.filename().string();
        //
        //                if (fileIt.is_directory()) // Folders within the directory
        //                {
        //                    ListFolders(std::filesystem::absolute(path));
        //                }
        //            }
        //            ImGui::TreePop(); // Close the dropdown
        //        }
        //    }
        //
        //}

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
            ImGuiButtonFlags flags;

            ImGui::BeginChild("Files", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);

            float panelWidth = ImGui::GetContentRegionAvail().x;
            float cellSize = 80 + 10; //first float is size, second is padding
            int columnCount = std::max(1, (int)(panelWidth / cellSize));
            if (ImGui::BeginTable("ContentBrowserTable", columnCount))
            {
                for (auto& it : std::filesystem::directory_iterator(m_SelectedPath))
                {
                    ListFiles(it);
                }

                //if left mouse button is pressed, clear the context
                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                {
                    m_SelectedFile = "";
                }


                RightClickMenu();
                ImGui::EndTable();
                
            
            }

            ImGui::EndChild();
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
                {
                    if (std::filesystem::exists(m_SelectedPath))
                    {
                        try
                        {
                            Entity* droppedEnt = static_cast<Entity*>(payload->Data); // Extract glm::vec3 pointer
                            //SceneSerialiser serialiser(m_SelectedPath, (uint32_t)droppedEnt);
                            SceneSerialiser serialiser(m_Context);

                            std::string tag = droppedEnt->GetComponent<TagComponent>().Tag;
                            std::replace(tag.begin(), tag.end(), ' ', '_'); // Replace spaces with underscore

                            serialiser.EntitySerialise(m_SelectedPath.string() + "\\" + tag, (entt::entity)*droppedEnt);
                        }
                        catch (...)
                        {

                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
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
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
                    {
                        try
                        {
                            std::filesystem::path dropped = static_cast<const char*>(payload->Data);
                            std::filesystem::path targetPath = path / dropped.filename(); // Destination path
                            std::filesystem::rename(dropped, targetPath); //rename its directory

                        }
                        catch (...)
                        {

                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::IsItemClicked())
                {
                    m_SelectedPath = std::filesystem::absolute(path);
                    LOG_INFO(std::filesystem::absolute(path).string());
                }

                FolderOptions(path);


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

    void ContentPanel::FolderOptions(std::filesystem::path path)
    {
        if (ImGui::BeginPopupContextItem(path.string().c_str()))
        {
            ImGUILibrary::DrawMenuItem("Create Folder", [this, path](Ref<Scene> m_Context) { FileSystem::CreateFolder(path, "New Folder"); }, m_Context);

            if (ImGui::BeginMenu("Rename Current Folder"))
            {
                static char inputBuffer[128] = "\0";


                std::filesystem::path newPath = ((std::filesystem::path)path);

                ImGui::InputTextWithHint("##NewName", "File Name...", inputBuffer, IM_ARRAYSIZE(inputBuffer));

                if (ImGui::Button("Apply"))
                {
                    newPath.replace_filename(std::string(inputBuffer));


                    try 
                    {
                        std::filesystem::rename(path, newPath);
                        std::cout << "File renamed successfully to " << newPath.string() << ".\n";
                        m_SelectedPath = "";
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        std::cerr << "Error renaming the file: " << e.what() << '\n';
                    }


                }

                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Delete"))
            {
                ImGui::Text("THIS CANNOT BE UNDONE!\nProceed?");
                ImGUILibrary::DrawMenuItem("Yes", [this, path](Ref<Scene> m_Context) {

                    m_SelectedPath = "";

                    if (std::filesystem::remove(path) == 0) {
                        LOG_WARN("File deleted successfully.\n");
                    }
                    else
                    {
                        LOG_ERROR("Error deleting the file");
                    }

                    }, m_Context);

                ImGui::EndMenu(); // End "Create Object"
            }


            ImGui::EndPopup();
        }
    }

    void ContentPanel::ListFiles(const std::filesystem::directory_entry& it)
    {
        const auto& path = it.path(); // the path of the current iterator

        //std::filesystem::relative(path, Project::currCodeProj);
        auto relativePathString = path.string(); //string of the path we are looking at

        ImGui::PushID(relativePathString.c_str());
        if (!it.is_directory())// if the iterator is not a file
        {
            ImGui::TableNextColumn();


            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            GLuint texture = Project::LoadTextureFromFile("src/Extra/Icons/UI/file.png");

            if (m_SelectedFile == path.string()) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.6f, 0.5f)); // Highlight color
            }
            if (ImGui::ImageButton(path.filename().string().c_str(), (ImTextureID)(intptr_t)texture, ImVec2(60.0f, 60.0f)))
            {
                //if (m_SelectedFile != path.string())
                m_SelectedFile = path.string();
                //else
                //{
                //    system(("start " + relativePathString).c_str()); // Opens with the default program
                //}
            }
            if (m_SelectedFile == path.string()) {
                ImGui::PopStyleColor(); // Restore default color
            }


            //Allow the ability to drag files
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("FILE_PATH", relativePathString.c_str(), relativePathString.size() + 1);
                ImGui::EndDragDropSource();
            }
            if (ImGui::IsItemClicked())
            {
                m_SelectedFile = path.string();
            }


            if (ImGui::BeginPopupContextItem())
            {
                m_SelectedFile = path.string();

                ImGUILibrary::DrawMenuItem("Open", [this](Ref<Scene> m_Context) { system(("start " + m_SelectedFile).c_str()); }, m_Context);

                if (ImGui::BeginMenu("Rename"))
                {
                    static char inputBuffer[128] = "";


                    std::filesystem::path newPath = (std::filesystem::path)m_SelectedFile;
                    std::string fileExtension = newPath.extension().string();

                    ImGui::InputTextWithHint("##NewName", "File Name...", inputBuffer, IM_ARRAYSIZE(inputBuffer));

                    if (ImGui::Button("Apply"))
                    {
                        newPath.replace_filename(std::string(inputBuffer) + fileExtension);

                        //if (std::rename( m_SelectedFile.c_str(), newPath.string().c_str()) == 0) {
                        //    std::cout << "File renamed successfully.\n";
                        //}
                        //else 
                        //{
                        //    LOG_ERROR("Error renaming the file");
                        //}
                        try {
                            std::filesystem::rename(m_SelectedFile, newPath);
                            std::cout << "File renamed successfully to " << newPath.string() << ".\n";
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "Error renaming the file: " << e.what() << '\n';
                        }


                    }

                    ImGui::EndMenu(); // End "Create Object"
                }


                if (ImGui::BeginMenu("Delete"))
                {
                    ImGui::Text("THIS CANNOT BE UNDONE!\nProceed?");
                    ImGUILibrary::DrawMenuItem("Yes", [this](Ref<Scene> m_Context) {

                        if (std::filesystem::remove(m_SelectedFile) == 0) {
                            LOG_WARN("File deleted successfully.\n");
                        }
                        else
                        {
                            LOG_ERROR("Error deleting the file");
                        }

                        }, m_Context);

                    ImGui::EndMenu(); // End "Create Object"
                }

                ImGui::EndPopup();
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

        /*
        if (m_SelectedFile != "")
        {

            if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_MouseButtonRight))
            {
                ImGUILibrary::DrawMenuItem("Open", [this](Ref<Scene> m_Context) { system(("start " + m_SelectedFile).c_str()); }, m_Context);

                if (ImGui::BeginMenu("Rename"))
                {
                    static char inputBuffer[128] = "";


                    std::filesystem::path newPath = (std::filesystem::path)m_SelectedFile;
                    std::string fileExtension = newPath.extension().string();

                    ImGui::InputTextWithHint("##NewName", "File Name...", inputBuffer, IM_ARRAYSIZE(inputBuffer));

                    if (ImGui::Button("Apply"))
                    {
                        newPath.replace_filename(std::string(inputBuffer) + fileExtension);

                        //if (std::rename( m_SelectedFile.c_str(), newPath.string().c_str()) == 0) {
                        //    std::cout << "File renamed successfully.\n";
                        //}
                        //else 
                        //{
                        //    LOG_ERROR("Error renaming the file");
                        //}
                        try {
                            std::filesystem::rename(m_SelectedFile, newPath);
                            std::cout << "File renamed successfully to " << newPath.string() << ".\n";
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "Error renaming the file: " << e.what() << '\n';
                        }


                    }

                    ImGui::EndMenu(); // End "Create Object"
                }


                if (ImGui::BeginMenu("Delete"))
                {
                    ImGui::Text("THIS CANNOT BE UNDONE!\nProceed?");
                    ImGUILibrary::DrawMenuItem("Yes", [this](Ref<Scene> m_Context) { 

                        if (std::filesystem::remove(m_SelectedFile) == 0) {
                            LOG_WARN("File deleted successfully.\n");
                        }
                        else
                        {
                            LOG_ERROR("Error deleting the file");
                        }

                    }, m_Context);

                    ImGui::EndMenu(); // End "Create Object"
                }

                ImGui::EndPopup();
            }
        }
        */

        if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
        {
            ImGUILibrary::DrawMenuItem("Create Folder", [this](Ref<Scene> m_Context) { FileSystem::CreateFolder(m_SelectedPath, "New Folder"); }, m_Context);
            
            if (ImGui::BeginMenu("Rename Current Folder"))
            {
                static char inputBuffer[128] = "\0";


                std::filesystem::path newPath = ((std::filesystem::path)m_SelectedPath);

                ImGui::InputTextWithHint("##NewName", "File Name...", inputBuffer, IM_ARRAYSIZE(inputBuffer));

                if (ImGui::Button("Apply"))
                {
                    newPath.replace_filename(std::string(inputBuffer));

                    //if (std::rename( m_SelectedFile.c_str(), newPath.string().c_str()) == 0) {
                    //    std::cout << "File renamed successfully.\n";
                    //}
                    //else 
                    //{
                    //    LOG_ERROR("Error renaming the file");
                    //}
                    try {
                        std::filesystem::rename(m_SelectedPath, newPath);
                        std::cout << "File renamed successfully to " << newPath.string() << ".\n";
                        m_SelectedPath = "";
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        std::cerr << "Error renaming the file: " << e.what() << '\n';
                    }


                }

                ImGui::EndMenu(); // End "Create Object"
            }


            ImGui::EndPopup();
        }
    }

}