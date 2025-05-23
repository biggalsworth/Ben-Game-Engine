#include "Engine_PCH.h"
#include "SceneHierarchyPanel.h"
#include "ImGui/ImGUILibrary.h"
#include <imgui/imgui.h>
#include "Scene/Components.h"
#include "Scene/SceneSerialiser.h"

namespace Engine
{
    SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene>& context)
    {
        SetContext(context);
        m_InspectorPanel = CreateRef<InspectorPanel>();
        //m_ContentPanel = new ContentPanel();
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_ContentPanel = new ContentPanel();
        

        m_Context = context;
        m_SelectionContext = {};

    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        if (m_Context)
        {
            if (ImGui::BeginMenu("Create Object"))//, ImVec2(ImGui::GetContentRegionAvail().x, 20)))
            {
                CreateMenu();
                ImGui::EndMenu();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
                {
                    try
                    {
                        std::filesystem::path dropped = static_cast<const char*>(payload->Data);
                        if (dropped.filename().extension() == ".EEF")
                        {
                            SceneSerialiser serialiser(m_Context);
                            serialiser.EntityLoad(dropped.string());
                        }
                    }
                    catch (...)
                    {

                    }
                }
                ImGui::EndDragDropTarget();
            }

            //get all entities in the scene and render them in the hierarchy
            auto view = m_Context->m_Registry.view<entt::entity>();
            for (auto entityID : view)
            {
                Entity entity{ entityID, m_Context.get() };
                //only draw it as a parent object if it has no parent
                if(entity.GetComponent<RelationshipComponent>().parent == entt::null)
                {
                    DrawEntityNode(entity);
                }
            }

            //if left mouse button is pressed, clear the context
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            {
                m_SelectionContext = {};
            }

            RightClickMenu();
        }
        ImGui::End();


        ImGui::Begin("Inspector");
        if (m_SelectionContext)
        {
            m_InspectorPanel->DrawComponents(m_SelectionContext);
            m_InspectorPanel->DrawAddComponent(m_SelectionContext);
        }
        ImGui::End();

        ImGui::Begin("Content View");
        m_ContentPanel->SetContext(m_Context);
        m_ContentPanel->Render();
        ImGui::End();


    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;
    }


    static bool isCurrentlyDragging = false; 
    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        //this will create a section that can be draggable
        ImGui::PushID((uint32_t)entity); // Ensures each item has a unique ID
        ImGui::Button(" ", { 5, 22 });
        ImGui::PopID(); // Restore ID scop
        // Allow for dragging entities
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(Entity));
            ImGui::EndDragDropSource();
        }
        //now draw the entity
        ImGui::SameLine();

        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str()); //add the entity to the scene hierarchy

        // If the item is clicked (without dragging), select it
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity; // Select the entity
        }
        



        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {

            if (ImGui::BeginMenu("Create Child"))
            {
                if (ImGui::BeginMenu("Create Sprite"))  // Parent menu item
                {
                    //ImGUILibrary::DrawMenuItem("Create Empty", [this, entity](Ref<Scene> m_Context) {
                    //
                    //    Entity empty = m_Context->CreateEntity("Empty Entity");
                    //    RelationshipComponent& comp = empty.GetComponent<RelationshipComponent>();
                    //    comp.parent = &entity;
                    //    }, m_Context);

                    if (ImGui::MenuItem("Create Empty")) // Child menu item
                    {
                        Entity square = m_Context->CreateEntity("Empty");
                        square.GetComponent<RelationshipComponent>().parent = (uint32_t)entity;
                    }

                    if (ImGui::MenuItem("Create Square")) // Child menu item
                    {
                        Entity square = m_Context->CreateEntity("Square");
                        square.AddComponent<SpriteRendererComponent>();
                        square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                        square.GetComponent<RelationshipComponent>().parent = (uint32_t)entity;
                        //square.SetLocal();

                    }

                    if (ImGui::MenuItem("Create Triangle")) // Child menu item
                    {
                        Entity square = m_Context->CreateEntity("Triangle");
                        square.AddComponent<SpriteRendererComponent>();
                        square.GetComponent<SpriteRendererComponent>().meshType = Triangle;
                        square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                        square.GetComponent<RelationshipComponent>().parent = (uint32_t)entity;

                    }

                    ImGui::EndMenu(); // End "Create Object"
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Duplicate Entity"))
            {
                m_Context->DuplicateEntity(entity);
            }

            if (ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            //check for children
            //auto view = m_Context->m_Registry.view<entt::entity>();
            auto& view = m_Context->GetAllEntitiesWith<RelationshipComponent>();
            for (auto& entt : view)
            {
                Entity otherEnt{ entt, m_Context.get() };

                if (otherEnt.HasComponent<RelationshipComponent>() == true)
                {
                    auto& comp = otherEnt.GetComponent<RelationshipComponent>();

                    if (comp.parent != entt::null && (uint32_t)comp.parent == (uint32_t)entity)
                    {
                        DrawEntityNode(otherEnt); // Recursively render child nodes
                    }
                }
            }
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity)
            {
                m_SelectionContext = {};
            }
        }
    }

    //on right click of the menu, create a popup menu
    //draw each menu item as part of the popup and when they are clicked, pass if the context was on this menu
    //if it was the menu that was clicked, call the function in the curly brackets
    void SceneHierarchyPanel::RightClickMenu()
    {
        //begin popup context window: 0 is a null value, this function will assign this an id based on what context was clicked
        //1 is checking for the right mouse click
        if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
        {
            ImGUILibrary::DrawMenuItem("Create Empty", [this](Ref<Scene> m_Context) { m_Context->CreateEntity("Empty Entity"); }, m_Context);
            
            if (ImGui::BeginMenu("Create Sprite"))  // Parent menu item
            {
                //if (ImGui::MenuItem("Create Camera")) // Child menu item
                //{
                //    Entity cam = m_Context->CreateEntity("Camera");
                //    cam.AddComponent<CameraComponent>();
                //}

                if (ImGui::MenuItem("Create Square")) // Child menu item
                {
                    Entity square = m_Context->CreateEntity("Square");
                    square.AddComponent<SpriteRendererComponent>();
                    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                }

                if (ImGui::MenuItem("Create Triangle")) // Child menu item
                {
                    Entity square = m_Context->CreateEntity("Triangle");
                    square.AddComponent<SpriteRendererComponent>();
                    square.GetComponent<SpriteRendererComponent>().meshType = Triangle;
                    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                }

                ImGui::EndMenu(); // End "Create Object"
            }

            //ImGUILibrary::DrawMenuItem("Create Square", [this](Ref<Scene> m_Context) {
            //    Entity square = m_Context->CreateEntity("Square");
            //    square.AddComponent<SpriteRendererComponent>();
            //    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
            //}, m_Context);

            ImGUILibrary::DrawMenuItem("Camera", [this](Ref<Scene> m_Context) {

                auto cams = m_Context->GetAllEntitiesWith<CameraComponent>();
                std::string name = "Main Camera";
                bool primary = true;
                if (cams.size() > 0)
                {
                    name = "Camera";
                    primary = false;
                }

                Entity cam = m_Context->CreateEntity(name);
                CameraComponent& c = cam.AddComponent<CameraComponent>();
                c.Primary = primary;
                }, m_Context);

            ImGui::EndPopup();
        }
    }
    void SceneHierarchyPanel::CreateMenu()
    {
            ImGUILibrary::DrawMenuItem("Create Empty", [this](Ref<Scene> m_Context) { m_Context->CreateEntity("Empty Entity"); }, m_Context);

            if (ImGui::BeginMenu("Create Sprite"))  // Parent menu item
            {
                //if (ImGui::MenuItem("Create Camera")) // Child menu item
                //{
                //    Entity cam = m_Context->CreateEntity("Camera");
                //    cam.AddComponent<CameraComponent>();
                //}

                if (ImGui::MenuItem("Create Square")) // Child menu item
                {
                    Entity square = m_Context->CreateEntity("Square");
                    square.AddComponent<SpriteRendererComponent>();
                    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                }

                if (ImGui::MenuItem("Create Triangle")) // Child menu item
                {
                    Entity square = m_Context->CreateEntity("Triangle");
                    square.AddComponent<SpriteRendererComponent>();
                    square.GetComponent<SpriteRendererComponent>().meshType = Triangle;
                    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
                }

                ImGui::EndMenu(); // End "Create Object"
            }

            //ImGUILibrary::DrawMenuItem("Create Square", [this](Ref<Scene> m_Context) {
            //    Entity square = m_Context->CreateEntity("Square");
            //    square.AddComponent<SpriteRendererComponent>();
            //    square.GetComponent<TransformComponent>().Scale = glm::vec3(1.0f, 1.0f, 1.0f);
            //}, m_Context);

            ImGUILibrary::DrawMenuItem("Camera", [this](Ref<Scene> m_Context) {

                auto cams = m_Context->GetAllEntitiesWith<CameraComponent>();
                std::string name = "Main Camera";
                bool primary = true;
                if (cams.size() > 0)
                {
                    name = "Camera";
                    primary = false;
                }

                Entity cam = m_Context->CreateEntity(name);
                CameraComponent& c = cam.AddComponent<CameraComponent>();
                c.Primary = primary;
                }, m_Context);

    }
}



