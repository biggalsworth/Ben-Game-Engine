#include "Engine_PCH.h"
#include "InspectorPanel.h"
#include "Editor/Project.h"


#include "ImGui/ImGuiLibrary.h"
#include "Editor/Project.h"
#include <imgui/imgui.h>
#include "Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include "ContentPanel.h"
#include <Script/ScriptGlue.h>

namespace Engine
{


    static void DrawScriptComponent(Entity entity, ScriptComponent* component, int i)
    {
        ImGui::Text("Script File:");
        ImGui::SameLine();
        ImGui::PushID(i);
        if (ImGui::Selectable(component->className.c_str() )) {
            if (std::filesystem::exists(std::filesystem::path(component->filePath)))
                system(("start " + component->filePath).c_str()); // Opens with the default program
        }
        if (ImGui::BeginDragDropTarget())
        {
            //Drag drop logic
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
            {
                try
                {
                    std::filesystem::path dropped = static_cast<const char*>(payload->Data);
                    if (dropped.filename().extension() == ".cs")
                    {
                        component->className = dropped.filename().string();
                        component->AssignScript(dropped);
                    }

                    //component->filePath = dropp6ed->string();
                }
                catch (...)
                {

                }
            }
            ImGui::EndDragDropTarget();

        }
        ImGui::PopID();

        #pragma region Class Finder

        //  std::string outputString = "";
        //  ImGUILibrary::DrawSearchWindow("Select Script: ", &outputString, ".cs");


        static char searchBuffer[128]; // Buffer for storing search text
        static std::vector<std::filesystem::path>* fileList = new std::vector<std::filesystem::path>(); // file names
        // Browse button for selecting a new script file (requires a file picker)
        if (ImGui::Button(("Select Script " + std::to_string(i) + " Class : ").c_str()))
        {
            ImGui::OpenPopup("BrowseClasses" + i);
            searchBuffer[0] = '\0'; // Empty the buffer
            //currentDir = std::filesystem::path("assets");
            fileList->clear();
        }
        if (ImGui::BeginPopup("BrowseClasses" + i))
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
                    if (it.filename().extension() == ".cs")
                    {
                        if (ImGui::MenuItem(it.filename().string().c_str()))  // Corrected MenuItem function call
                        {
                            component->className = it.filename().string();
                            component->AssignScript(it);
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

                    if (path.extension() == ".cs")
                    {
                        if (ImGui::MenuItem(relativePathString.c_str()))
                        {
                            component->className = path.filename().string();
                            component->AssignScript(path);
                            std::cout << relativePathString.c_str() << std::endl;
                        }
                    }
                }



                for (auto& it : std::filesystem::recursive_directory_iterator(Project::currCodeProj / "Assets"))
                {
                    const auto& path = it.path(); // the path of the current iterator
                    //std::filesystem::relative(path, std::filesystem::path("Assets"));
                    auto relativePathString = path.string(); //string of the path we are looking at

                    if (path.extension() == ".cs")
                    {
                        if (ImGui::MenuItem(path.filename().string().c_str()))
                        {
                            component->className = path.filename().string();
                            component->AssignScript(path);
                            std::cout << relativePathString.c_str() << std::endl;

                        }
                    }
                }
            }

            if (ImGui::Button("Create Class"))
            {
                ImGui::OpenPopup("ClassCreation");
                fileList->clear();
            }

            if (ImGui::BeginPopup("ClassCreation"))
            {
                ImGui::InputTextWithHint("##ClassCreation", "Class Name (NO NEED TO PUT .cs)...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

                if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                {
                    if (searchBuffer[0] != '\0')
                    {
                        component->CreateScript(searchBuffer);
                        MonoManager::CompileScripts();
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::EndPopup();

        }

#pragma endregion


        //show fields within the script
        bool sceneRunning = true;
        Ref<ScriptInstance> scriptInstance = component->Script;
        Ref<ScriptClass> Script = MonoManager::EntityClasses[component->className];
        if (entity.GetScene()->isRunning)
        {
            Ref<ScriptInstance> scriptInstance = component->Script;
            if (scriptInstance)
            {
                const auto& fields = scriptInstance->GetScriptClass()->GetFields();
                for (const auto& [name, field] : fields)
                {
                    if (field.Type == ScriptFieldType::Float)
                    {
                        float data = scriptInstance->GetFieldValue<float>(name);
                        if (ImGui::DragFloat(name.c_str(), &data))
                        {
                            scriptInstance->SetFieldValue(name, data);
                        }
                    }

                    if (field.Type == ScriptFieldType::Int)
                    {
                        int data = scriptInstance->GetFieldValue<int>(name);
                        if (ImGui::DragInt(name.c_str(), &data))
                        {
                            scriptInstance->SetFieldValue(name, data);
                        }
                    }
                    if (field.Type == ScriptFieldType::Bool)
                    {
                        bool data = scriptInstance->GetFieldValue<bool>(name);
                        if (ImGui::Checkbox(name.c_str(), &data))
                        {
                            scriptInstance->SetFieldValue(name, data);
                        }
                    }


                    if (field.Type == ScriptFieldType::string)
                    {

                        MonoString* data = scriptInstance->GetFieldValue<MonoString*>(name);
                        int strLength = mono_string_length(data);
                        char* buffer = mono_string_to_utf8(data);

                        if (ImGui::InputText(name.c_str(), buffer, strLength, ImGuiInputTextFlags_EnterReturnsTrue) == true)
                        {
                            std::string str(buffer);
                            mono_free(buffer);
                            data = mono_string_new(MonoManager::domain, str.c_str());
                            scriptInstance->SetFieldValue(name, data);

                        }
                    }
                }
            }
        }
        else
        {
            if (Script != nullptr)
            {

                const auto& fields = Script->GetFields();

                auto& entityFields = MonoManager::GetScriptFieldMap(entity);
                for (const auto& [name, field] : fields)
                {
                    // Field has been set in editor
                    if (entityFields.find(name) != entityFields.end())
                    {
                        ScriptFieldInstance& scriptField = entityFields.at(name);

                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::Float)
                        {
                            float data = scriptField.GetValue<float>();
                            if (ImGui::DragFloat(name.c_str(), &data))
                                scriptField.SetValue(data);
                        }
                        if (field.Type == ScriptFieldType::Int)
                        {
                            int data = scriptField.GetValue<int>();
                            if (ImGui::DragInt(name.c_str(), &data))
                                scriptField.SetValue(data);

                        }
                        if (field.Type == ScriptFieldType::Bool)
                        {
                            bool data = scriptField.GetValue<bool>();
                            if (ImGui::Checkbox(name.c_str(), &data))
                                scriptField.SetValue(data);
                        }

                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::string)
                        {
                            MonoString* data = scriptField.GetValue<MonoString*>();
                            int strLength = mono_string_length(data);
                            char* buffer = mono_string_to_utf8(data);

                            if (ImGui::InputText(name.c_str(), buffer, strLength, ImGuiInputTextFlags_EnterReturnsTrue) == true)
                            {
                                std::string str(buffer);
                                mono_free(buffer);
                                data = mono_string_new(MonoManager::domain, str.c_str());
                                scriptField.SetValue(data);
                            }
                        }
                    }
                    else
                    {
                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::Float)
                        {
                            float data = 0.0f;
                            if (ImGui::DragFloat(name.c_str(), &data))
                            {
                                ScriptFieldInstance& fieldInstance = entityFields[name];
                                fieldInstance.Field = field;
                                fieldInstance.SetValue(data);
                            }
                        }
                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::Int)
                        {
                            int data = 0;
                            if (ImGui::DragInt(name.c_str(), &data))
                            {
                                ScriptFieldInstance& fieldInstance = entityFields[name];
                                fieldInstance.Field = field;
                                fieldInstance.SetValue(data);
                            }
                        }
                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::Bool)
                        {
                            bool data = false;
                            if (ImGui::Checkbox(name.c_str(), &data))
                            {
                                ScriptFieldInstance& fieldInstance = entityFields[name];
                                fieldInstance.Field = field;
                                fieldInstance.SetValue(data);
                            }
                        }
                        // Display control to set it maybe
                        if (field.Type == ScriptFieldType::string)
                        {
                            char buffer[30] = {};
                            MonoString* data;
                            if (ImGui::InputText(name.c_str(), buffer, 30, ImGuiInputTextFlags_EnterReturnsTrue) == true)
                            {
                                data = mono_string_new(MonoManager::domain, buffer);

                                ScriptFieldInstance& fieldInstance = entityFields[name];
                                fieldInstance.Field = field;
                                fieldInstance.SetValue(data);

                            }
                        }
                    }
                }
            }
        }
        //}
    }


    void InspectorPanel::DrawAddComponent(Entity ent)
    {
        ImGui::Separator();
        ImGUILibrary::DrawCentredButton("Add Component", 0.9f, []()
            {
                ImGui::OpenPopup("AddComponent");
            });

        if (ImGui::BeginPopup("AddComponent"))
        {
            
            DrawAddScript(ent);
            //DrawAddComponentMenuItem<ScriptComponent>(ent, "New Script");

            DrawAddComponentMenuItem<SpriteRendererComponent>(ent, "Sprite Renderer");
            DrawAddComponentMenuItem<CircleRendererComponent>(ent, "Circle Renderer");
            DrawAddComponentMenuItem<AnimationComponent>(ent, "Animator");
            DrawAddComponentMenuItem <CameraComponent>(ent, "Camera");

            if (ImGui::BeginMenu("Audio"))
            {
                DrawAddComponentMenuItem <AudioListenerComponent>(ent, "Audio Listener");
                DrawAddComponentMenuItem <AudioSourceComponent>(ent, "Audio Source");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Physics"))
            {
                DrawAddComponentMenuItem <Rigidbody2DComponent>(ent, "Rigidbody 2D");
                //DrawAddComponentMenuItem <Collider2DComponent>(ent, "Collider 2D");
                DrawAddComponentMenuItem <BoxCollider2DComponent>(ent, "Box Collider 2D");
                DrawAddComponentMenuItem <CircleCollider2DComponent>(ent, "Circle Collider 2D");

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Networking"))
            {
                DrawAddComponentMenuItem <NetworkClientComponent>(ent, "Network Client");
                //DrawAddComponentMenuItem <Collider2DComponent>(ent, "Collider 2D");
                DrawAddComponentMenuItem <NetworkHostComponent>(ent, "Network Host");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("AI"))
            {
                DrawAddComponentMenuItem <NavMeshComponent>(ent, "Navmesh Generator");
                DrawAddComponentMenuItem <AINavigatorComponent>(ent, "AI Navigator");

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    }

    void InspectorPanel::DrawAddScript(Entity ent)
    {
        if (ImGui::MenuItem("New Script: "))
        {
            ent.GetComponent<ScriptComponentManager>().AddScript();// <ScriptComponent>();

            ImGui::CloseCurrentPopup();
        }
    }
    
    template<typename T>
    void InspectorPanel::DrawAddComponentMenuItem(Entity ent, const char* label)
    {
        if (!ent.HasComponent<T>())
        {
            if (ImGui::MenuItem(label))
            {
                ent.AddComponent<T>();

                ImGui::CloseCurrentPopup();
            }
        }
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGUILibrary::DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
            {
                ImGUILibrary::DrawVec3Control("Translation", component.Translation);
                glm::vec3 rotation = glm::degrees(component.Rotation);
                ImGUILibrary::DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);
                ImGUILibrary::DrawVec3Control("Scale", component.Scale, 1.0f);
            });


        //dont make it deletable
        ImGUILibrary::DrawComponent<ScriptComponentManager>("Script Manager", entity, [entity](auto& component)//ImGUILibrary::DrawComponent<ScriptComponentManager>("Script Manager", entity, [entity](auto& component)
            {
                //ScriptComponentManager component = entity.GetComponent<ScriptComponentManager>();
                if (ImGui::Button("Add Script "))
                {
                    entity.GetComponent<ScriptComponentManager>().AddScript();
                }

                ImGui::Separator();


                int i = 0;
                for (ScriptComponent* script : component.Scripts)
                {
                    //draw each script component
                    DrawScriptComponent(entity, script, i);
                    ImGui::PushID(i);
                    if(ImGui::Button(("Delete Script " + script->className).c_str()))
                    {
                        entity.GetComponent<ScriptComponentManager>().Scripts.erase(component.Scripts.begin() + i);
                        delete script;
                    }
                    ImGui::PopID();
                    ImGui::Separator();
                    i++;
                }
            });

        ImGUILibrary::DrawComponent<CameraComponent>("Camera", entity, [&](auto& component)
            {
                auto& camera = component.camera;

                const char* projectionStringTypes[] = { "Orthographic", "Perspective" };
                const char* currentProjectionString = projectionStringTypes[(int)camera.GetProjectionType()];
                if (ImGui::BeginCombo("Camera Type", currentProjectionString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentProjectionString == projectionStringTypes[i];
                        if (ImGui::Selectable(projectionStringTypes[i], isSelected))
                        {
                            currentProjectionString = projectionStringTypes[i];
                            camera.SetProjectionType((ProjectionType)i);

                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }
                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &orthoSize, 0.1f))
                    {
                        camera.SetOrthographicSize(orthoSize);
                    }

                    float orthoNear = camera.GetOrthographicNear();
                    if (ImGui::DragFloat("Near", &orthoNear, 0.1f))
                    {
                        camera.SetOrthographicNear(orthoNear);
                    }

                    float orthoFar = camera.GetOrthographicFar();
                    if (ImGui::DragFloat("Far", &orthoFar, 0.1f))
                    {
                        camera.SetOrthographicFar(orthoFar);
                    }
                    ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
                }

                if (camera.GetProjectionType() == ProjectionType::Perspective)
                {
                    float perspectiveFov = glm::degrees(camera.GetPerspectiveFOV());
                    if (ImGui::DragFloat("FOV", &perspectiveFov, 0.1f))
                    {
                        camera.SetPerspectiveFOV(glm::radians(perspectiveFov));
                    }

                    float perspectiveNear = camera.GetPerspectiveNear();
                    if (ImGui::DragFloat("Near", &perspectiveNear, 0.1f))
                    {
                        camera.SetPerspectiveNear(perspectiveNear);
                    }

                    float perspectiveFar = camera.GetPerspectiveFar();
                    if (ImGui::DragFloat("Far", &perspectiveFar, 0.1f))
                    {
                        camera.SetPerspectiveFar(perspectiveFar);
                    }
                }
            });




        ImGUILibrary::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
            {
                static char searchBuffer[128]; // Buffer for storing search text
                static std::filesystem::path currentDir;

                ImGui::Text("Drop File Here:");
                if (ImGui::Button(std::string("Current Texture: " + component.fileName).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 25)))
                {
                    system(("start " + component.texture->GetPath()).c_str()); // Opens with default program
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
                    {
                        try
                        {
                            std::filesystem::path dropped = static_cast<const char*>(payload->Data);
                            if (dropped.filename().extension() == ".png")
                            {
                                //component.soundFile = FileSystem::FindRelativeToProject(dropped).string();
                                component.fileName = dropped.filename().string();
                                component.texture = Project::GetTextureLibrary()->CreateTexture(dropped.string());

                            }
                        }
                        catch (...)
                        {

                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Dummy(ImVec2{ 0, 10 });


                std::string output = "";
                ImGUILibrary::DrawSearchWindow("Select Texture", &output, ".png");
                if (output != "")
                {
                    component.fileName = std::filesystem::path(output).filename().string();

                    component.texture = Project::GetTextureLibrary()->CreateTexture(output);
                }

                ImGui::Separator();

                ImGui::Dummy(ImVec2{ 0, 10 });

                ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));

                ImGui::Dummy(ImVec2{ 0, 10 });

                ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 10.0f);
            });



        ImGUILibrary::DrawComponent<ScriptComponent>("Script Component", entity, [entity](auto& component)
            {
                //DrawScriptComponent(entity, *component, 0);
             });

        


        ImGUILibrary::DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
            {
                //TODO: add texture and tiling fields
                ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));
                //ImGui::DragFloat("Radius", &component.Radius, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
            });

        ImGUILibrary::DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
            {
                const char* bodyTypes[] = { "Static", "Kinematic", "Dynamic" };
                const char* currentBodyType = bodyTypes[(int)component.Type];

                if (ImGui::BeginCombo("Rigidbody Type", currentBodyType))
                {
                    for (int i = 0; i < 3; i++)
                    {
                        bool isSelected = currentBodyType == bodyTypes[i];
                        if (ImGui::Selectable(bodyTypes[i], isSelected))
                        {
                            currentBodyType = bodyTypes[i];
                            component.Type = ((Rigidbody2DComponent::BodyType)i);

                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
                ImGui::DragFloat("Gravity Scale", &component.GravityScale, 0.1f, 0.0f, 1.0f);
                ImGui::DragFloat("Mass", &component.Mass, 0.1f, 0.0f, 100.0f);
                ImGui::DragFloat("Linear Damping", &component.LinearDamping, 0.1f, 0.001f, 5.0f);
                ImGui::DragFloat("Angular Damping", &component.AngularDamping, 0.1f, 0.0f, 5.0f);

                //ImGui::SliderFloat2("Size", glm::value_ptr(component.Size), 0.1f, 5.0f);
                //ImGui::SliderFloat2("Offset", glm::value_ptr(component.Offset),  0.0f, INFINITE);
                //
                //ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f, 10.0f);
                //ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                //ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);

            });


        //ImGUILibrary::DrawComponent<Collider2DComponent>("Collider 2D", entity, [](auto& component)
        //    {
        //        //ImGui::SliderFloat2("Size", glm::value_ptr(component.Size), 0.1f, 5.0f);
        //        //ImGui::SliderFloat2("Offset", glm::value_ptr(component.Offset),  0.0f, INFINITE);
        //        const char* bodyTypes[] = { "Box", "Circle", "Capsule" };
        //        const ColliderTypes* type = component.Type;
        //        if (ImGui::BeginCombo("Collider Type", type))
        //        {
        //            for (int i = 0; i < 3; i++)
        //            {
        //                bool isSelected = type == bodyTypes[i];
        //                if (ImGui::Selectable(bodyTypes[i], isSelected))
        //                {
        //                    type = bodyTypes[i];
        //                    component.Type = ((ColliderTypes)i);

        //                    component.Update(component.Type)

        //                    if (isSelected)
        //                    {
        //                        ImGui::SetItemDefaultFocus();
        //                    }
        //                }
        //            }
        //            ImGui::EndCombo();
        //        }
        //        if (component.Type == ColliderTypes::Circle)
        //        {
        //            ImGui::InputFloat("Radius", glm::value_ptr(component.comp.Radius));

        //        }
        //        else
        //            ImGui::InputFloat2("Size", glm::value_ptr(component.Size));
        //        ImGui::InputFloat2("Offset", glm::value_ptr(component.Offset));
        //        
        //        ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f, 10.0f);
        //        ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
        //        ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);

        //    });

        ImGUILibrary::DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
            {
                //ImGui::SliderFloat2("Size", glm::value_ptr(component.Size), 0.1f, 5.0f);
                //ImGui::SliderFloat2("Offset", glm::value_ptr(component.Offset),  0.0f, INFINITE);

                ImGui::Checkbox("isTrigger", &component.isTrigger);
                ImGui::DragFloat2("Size", glm::value_ptr(component.Size), 0.1f, 0.1f, 10.0f);
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f, 0.1f, 10.0f);

                //ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 2.0f);

            });

        ImGUILibrary::DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
            {
                //ImGui::SliderFloat2("Size", glm::value_ptr(component.Size), 0.1f, 5.0f);
                //ImGui::SliderFloat2("Offset", glm::value_ptr(component.Offset),  0.0f, INFINITE);
                ImGui::Checkbox("isTrigger", &component.isTrigger);
                ImGui::DragFloat("Radius", &component.Radius, 0.01f, 0.0f);
                ImGUILibrary::DrawVec2Control("Offset", component.Offset, 0.0f, 50.0f);

                //ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 2.0f);

            });


        ImGUILibrary::DrawComponent<AnimationComponent>("Animation Component", entity, [entity](auto& component)
            {
                auto& animation = entity.GetComponent<AnimationComponent>();

                ImGui::DragFloat("Interval", &component.interval, 0.1f, 0.0f, 5.0f);

                //if (!animation.sprites.empty() && !animation.images.empty())
                //{
                //
                //    ImGui::Text("Image File:");
                //    ImGui::SameLine();
                //
                //    if (ImGui::Selectable(animation.images[0].filename().string().c_str())) {
                //        if (std::filesystem::exists(std::filesystem::path(animation.images[0])))
                //            system(("start " + animation.images[0].string()).c_str()); // Opens with the default program
                //    }
                //}
                //else if (!animation.images.empty())
                if (!animation.images.empty())
                {
                    int count = 0;
                    for (std::filesystem::path& image : component.images)
                    {
                        ImGui::Text("Image File:");
                        ImGui::SameLine();

                        if (ImGui::Selectable(image.filename().string().c_str())) {
                            if (std::filesystem::exists(std::filesystem::path(image)))
                                system(("start " + image.string()).c_str()); // Opens with the default program
                        }

                        GLuint textureID = Project::LoadTextureFromFile(image.string().c_str());
                        if (textureID)
                        {
                            ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2(100, 100)); // Show image at 200x200
                        }

                        if (ImGui::Button(("Delete Image " + std::to_string(count)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            animation.RemoveImage(count);
                        }
                        count++;
                    }
                }
                
                ImGui::Separator();

                if (ImGui::TreeNode("Sprite Sheet"))
                {
                    ImGUILibrary::DrawVec2Control("Sprite Cell Size", component.spriteCellSize, 1.0f, 120);
                    ImGUILibrary::DrawVec2Control("Row & Column Count", component.spriteCellTable, 0.0f, 120);

                    if (ImGui::Button("Generate Sprites", ImVec2(-1, 25)))
                    {
                        component.sprites = Renderer2D::GenerateSpriteSheet(Project::GetTextureLibrary()->CreateTexture(component.images[0].string()), 1, component.spriteCellTable, component.spriteCellSize);
                    }

                    ImGui::TreePop(); // Closes the node
                }


                std::string output = "";
                ImGUILibrary::DrawSearchWindow("New Image", &output, ".png");
                if (output != "")
                    entity.GetComponent<AnimationComponent>().AddImage(std::filesystem::path(output));

            });

        ImGUILibrary::DrawComponent<AudioListenerComponent>("Audio Listener", entity, [](auto& component)
            {
                ImGui::Text("Audio Listener is active");

            });


        ImGUILibrary::DrawComponent<AudioSourceComponent>("Audio Source", entity, [](auto& component)
            {

                //if (component.soundFile == "0")
                //{
                //    component.soundName = "NO AUDIO LOADED"
                //}

                ImGui::Text(std::string("Current Sound: " + component.soundName).c_str());
                if (ImGui::BeginDragDropTarget())
                {
                    //Drag drop logic
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
                    {
                        try
                        {
                            std::filesystem::path dropped = static_cast<const char*>(payload->Data);
                            if (dropped.filename().extension() == ".mp3")
                            {
                                component.soundFile = FileSystem::FindRelativeToProject(dropped).string();
                                component.soundName = dropped.filename().string();
                            }
                        }
                        catch (...)
                        {

                        }
                    }
                    ImGui::EndDragDropTarget();

                }

                static char searchBuffer[128]; // Buffer for storing search text
                static std::filesystem::path currentDir;
                //finding files
                static std::vector<std::filesystem::path>* fileList = new std::vector<std::filesystem::path>(); // file names

                if (ImGui::Button("Select Sound:  "))
                {
                    ImGui::OpenPopup("SoundSelection");
                    searchBuffer[0] = '\0'; // Empty the buffer
                    currentDir = std::filesystem::path("assets");
                    fileList->clear();
                }

                if (ImGui::BeginPopup("SoundSelection"))
                {
                    ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

                    ImGui::SameLine();

                    if (ImGui::Button("Search") || ImGui::IsKeyPressed(ImGuiKey_Enter))
                    {
                        if (searchBuffer[0] = '\0')
                        {
                            fileList->clear();
                        }
                        else
                        {
                            std::cout << searchBuffer << std::endl;
                            fileList = Project::FindFileInSubfolders(std::filesystem::path("assets"), searchBuffer);
                        }
                    }

#pragma region FileAccess
                    if (fileList->size() > 0)
                    {
                        for (int i = 0; i < fileList->size(); i++)
                        {
                            std::filesystem::path& it = (*fileList)[i];  // Get the current element in the list

                            if (ImGui::MenuItem(it.filename().string().c_str()))  // Corrected MenuItem function call
                            {
                                if (it.filename().extension() == ".mp3")
                                {
                                    //component.soundFile = it.string();
                                    component.soundFile = FileSystem::FindRelativeToProject(it).string();

                                    std::cout << "File:" << it.string() << std::endl;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (currentDir != std::filesystem::path("assets"))
                        {
                            if (ImGui::Button("<- Back"))
                            {
                                currentDir = currentDir.parent_path();
                            }
                        }

                        for (auto& it : std::filesystem::directory_iterator(currentDir))
                        {
                            const auto& path = it.path(); // the path of the current iterator
                            std::filesystem::relative(path, std::filesystem::path("assets"));
                            auto relativePathString = path.relative_path().string(); //string of the path we are looking at

                            if (it.is_directory())
                            {
                                if (ImGui::Button(relativePathString.c_str()))
                                {
                                    std::cout << relativePathString.c_str() << std::endl;
                                    currentDir /= path.filename();
                                }
                            }
                            else
                            {
                                if (ImGui::MenuItem(relativePathString.c_str()))
                                {
                                    if (path.extension() == ".mp3")
                                    {
                                        //component.soundFile = relativePathString;
                                        component.soundFile = FileSystem::FindRelativeToProject(path).string();

                                        std::cout << relativePathString.c_str() << std::endl;
                                    }
                                }
                            }
                        }
                    }
#pragma endregion

                    ImGui::EndPopup();

                }

                ImGui::Dummy({ 0.0f, 5.0f });
                ImGui::Checkbox("Is Looping", &component.loops);
                ImGui::Checkbox("Play On Awake", &component.OnAwake);
                ImGui::DragFloat("Volume", &component.volume, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Pitch", &component.pitch, 0.01f, 0.0f, 2.0f);

                ImGui::DragFloat("Maximum Range", &component.MaxRange, 1.0f, 0.0f, 200.0f);
                ImGui::DragFloat("Mininum Range", &component.MinRange, 1.0f, 0.0f, 100.0f);



            });


        ImGUILibrary::DrawComponent<NetworkClientComponent>("Network Client Component", entity, [](auto& component)
            {
                auto& address = component.IPAddress;
                auto& port = component.port;
                char buffer[256];
                char portBuffer[256];
                memset(buffer, 0, sizeof(buffer));
                memset(portBuffer, 0, sizeof(portBuffer));

                ImGui::Text("IP Address:");
                strcpy_s(buffer, sizeof(buffer), address.c_str());
                if (ImGui::InputText("##IP Address", buffer, sizeof(buffer)))
                {
                    address = std::string(buffer);
                }

                ImGui::Text("Port:");
                strcpy_s(portBuffer, sizeof(portBuffer), std::to_string(port).c_str());
                if (ImGui::InputText("##Port", portBuffer, sizeof(portBuffer)))
                {
                    int portInt = std::stoi(portBuffer);

                    // does value fit in a uint16_t
                    if (portInt < 0 || portInt > 65535)
                    {
                        std::cerr << "Error: Port number is out of range for uint16_t!" << std::endl;
                    }
                    else
                    {
                        port = static_cast<uint16_t>(portInt);
                    }

                }

                ImGui::Separator();

            });

        ImGUILibrary::DrawComponent<NetworkHostComponent>("Network Server Host", entity, [](auto& component)
            {
                auto& address = component.IPAddress;
                auto& port = component.port;
                char buffer[256];
                char portBuffer[256];
                memset(buffer, 0, sizeof(buffer));
                memset(portBuffer, 0, sizeof(portBuffer));

                ImGui::Text("IP Address:");
                strcpy_s(buffer, sizeof(buffer), address.c_str());
                if (ImGui::InputText("##IP Address", buffer, sizeof(buffer)))
                {
                    address = std::string(buffer);
                }

                ImGui::Text("Port:");
                strcpy_s(portBuffer, sizeof(portBuffer), std::to_string(port).c_str());
                if (ImGui::InputText("##Port", portBuffer, sizeof(portBuffer)))
                {
                    int portInt = std::stoi(portBuffer);

                    // does value fit in a uint16_t
                    if (portInt < 0 || portInt > 65535)
                    {
                        std::cerr << "Error: Port number is out of range for uint16_t!" << std::endl;
                    }
                    else
                    {
                        port = static_cast<uint16_t>(portInt);
                    }

                }

            });
	

        ImGUILibrary::DrawComponent<NavMeshComponent>("AI Navigator", entity, [](NavMeshComponent& component)
            {
                if (ImGui::Button("Generate Navmesh"))
                {
                    component.BeginGeneration();
                }
            });

        ImGUILibrary::DrawComponent<AINavigatorComponent>("AI Navigator", entity, [](AINavigatorComponent& component)
            {
                if (ImGui::Button("Add Point"))
                {
                    component.points.push_back(new glm::vec3{ 0.0f,0.0f,0.0f });
                }

                for(glm::vec3* pos : component.points)
                {
                    ImGui::Text("X: %.2f", pos->x);
                    ImGui::SameLine();
                    ImGui::Text("Y: %.2f", pos->y);


                    ImGui::PushID(pos); // Ensures each item has a unique ID to drag to
                    ImGui::Button("Drag Entity Here");
                    ImGui::PopID();
                    if (ImGui::BeginDragDropTarget())
                    {
                        //Drag drop logic
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
                        {
                            try
                            {
                                Entity* droppedEnt = static_cast<Entity*>(payload->Data); // Extract glm::vec3 pointer
                                pos->x = droppedEnt->GetComponent<TransformComponent>().Translation.x;
                                pos->y = droppedEnt->GetComponent<TransformComponent>().Translation.y;
                            }
                            catch (...)
                            {

                            }
                            ImGui::Text("Dropped Entity");
                        }
                        ImGui::EndDragDropTarget();

                    }

                }
            
                
            });


    }
}