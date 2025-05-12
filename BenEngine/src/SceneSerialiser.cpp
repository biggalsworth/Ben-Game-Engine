#include "Engine_PCH.h"
#include "Scene/SceneSerialiser.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utils/YamlConverter.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

#include "Editor/Project.h"

namespace Engine
{
    SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
    }

    static void SerialiseEntity(YAML::Emitter& out, Entity entity)
    {
        CORE_ASSERT("Entity does not have ID component", entity.HasComponent<IDComponent>());

        out << YAML::BeginMap; //Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        if (entity.HasComponent<RelationshipComponent>())
        {
            out << YAML::Key << "RelationshipComponent";
            out << YAML::BeginMap; // RelationshipComponent

            auto& relationship = entity.GetComponent<RelationshipComponent>();
            out << YAML::Key << "Parent" << YAML::Value << relationship.parent;

            out << YAML::EndMap; // RelationshipComponent
        }

        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; //TagComponent
        }

        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tr = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tr.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tr.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tr.Scale;

            out << YAML::EndMap; //TransformComponent
        }

        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNear();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFar();
            out << YAML::EndMap; // Camera

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }
        
        if (entity.HasComponent<AudioListenerComponent>())
        {
            out << YAML::Key << "AudioListenerComponent";
            out << YAML::BeginMap; // CameraComponent
            auto& listener = entity.GetComponent<AudioListenerComponent>();
            out << YAML::EndMap;

        }


        if (entity.HasComponent<AudioSourceComponent>())
        {
            out << YAML::Key << "AudioSourceComponent";
            out << YAML::BeginMap;

            auto& audioSource = entity.GetComponent<AudioSourceComponent>();

            out << YAML::Key << "position"  << YAML::Value << glm::vec3(audioSource.position.x, audioSource.position.y, audioSource.position.z);
            out << YAML::Key << "velocity"  << YAML::Value << glm::vec3(audioSource.velocity.x, audioSource.velocity.y, audioSource.velocity.z);
            out << YAML::Key << "soundFile" << YAML::Value << FileSystem::FindRelativeToProject((std::filesystem::path)audioSource.soundFile).string();
            out << YAML::Key << "volume"    << YAML::Value << (float)audioSource.volume;
            out << YAML::Key << "pitch"     << YAML::Value << (float)audioSource.pitch;
            out << YAML::Key << "MaxRange"  << YAML::Value << (float)audioSource.MaxRange;
            out << YAML::Key << "MinRange"  << YAML::Value << (float)audioSource.MinRange;
            out << YAML::Key << "loops"  << YAML::Value << (bool)audioSource.loops;
            out << YAML::Key << "OnAwake"  << YAML::Value << (bool)audioSource.OnAwake;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Colour" << YAML::Value << spriteRendererComponent.Colour;
            out << YAML::Key << "meshType" << YAML::Value << spriteRendererComponent.meshType;
            //out << YAML::Key << "Path" << YAML::Value << spriteRendererComponent.texture->GetPath();
            out << YAML::Key << "Path" << YAML::Value << FileSystem::FindRelativeToProject(spriteRendererComponent.texture->GetPath()).string();
            out << YAML::Key << "tilingFactor" << YAML::Value << spriteRendererComponent.tilingFactor;

            out << YAML::EndMap; // SpriteRendererComponent
        }

        if (entity.HasComponent<CircleRendererComponent>())
        {
            out << YAML::Key << "CircleRendererComponent";
            out << YAML::BeginMap;

            auto& circleRender = entity.GetComponent<CircleRendererComponent>();
            out << YAML::Key << "Colour" << YAML::Value << circleRender.Colour;
            out << YAML::Key << "Fade" << YAML::Value << circleRender.Fade;
            out << YAML::Key << "meshType" << YAML::Value << circleRender.meshType;
            out << YAML::Key << "Radius" << YAML::Value << circleRender.Radius;
            out << YAML::Key << "Thickness" << YAML::Value << circleRender.Thickness;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap;

            auto& rb = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "fixedRotation" << YAML::Value << rb.fixedRotation;
            out << YAML::Key << "GravityScale" << YAML::Value << rb.GravityScale;
            out << YAML::Key << "Mass" << YAML::Value << rb.Mass;
            out << YAML::Key << "LinearDamping" << YAML::Value << rb.LinearDamping;
            out << YAML::Key << "AngularDamping" << YAML::Value << rb.AngularDamping;
            out << YAML::Key << "Type" << YAML::Value << (int)rb.Type;
            out << YAML::Key << "RuntimeBody" << YAML::Value << rb.RuntimeBody;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;

            auto& box = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Friction" << YAML::Value << box.Friction;
            //out << YAML::Key << "Mass" << YAML::Value << box.Mass;
            out << YAML::Key << "Offset" << YAML::Value << box.Offset;
            out << YAML::Key << "Restitution" << YAML::Value << box.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << box.RestitutionThreshold;
            out << YAML::Key << "RuntimeFixture" << YAML::Value << box.RuntimeFixture;
            out << YAML::Key << "type" << YAML::Value << (int)box.type;
            out << YAML::Key << "isTrigger" << YAML::Value << box.isTrigger;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap;

            auto& circle = entity.GetComponent<CircleCollider2DComponent>();
            out << YAML::Key << "Friction" << YAML::Value << circle.Friction;
            //out << YAML::Key << "Mass" << YAML::Value << circle.Mass;
            out << YAML::Key << "Size" << YAML::Value << circle.Size;
            out << YAML::Key << "Offset" << YAML::Value << circle.Offset;
            out << YAML::Key << "Radius" << YAML::Value << circle.Radius;
            out << YAML::Key << "Restitution" << YAML::Value << circle.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << circle.RestitutionThreshold;
            out << YAML::Key << "RuntimeFixture" << YAML::Value << circle.RuntimeFixture;
            out << YAML::Key << "type" << YAML::Value << (int)circle.type;
            out << YAML::Key << "isTrigger" << YAML::Value << circle.isTrigger;

            out << YAML::EndMap;
        }
        if (entity.HasComponent<NetworkClientComponent>())
        {
            out << YAML::Key << "NetworkClientComponent";
            out << YAML::BeginMap;

            auto& comp = entity.GetComponent<NetworkClientComponent>();
            out << YAML::Key << "port" << YAML::Value << comp.port;
            out << YAML::Key << "IPAddress" << YAML::Value << comp.IPAddress;

            out << YAML::EndMap;
        }
        if (entity.HasComponent<NetworkHostComponent>())
        {
            out << YAML::Key << "NetworkHostComponent";
            out << YAML::BeginMap;

            auto& comp = entity.GetComponent<NetworkHostComponent>();
            out << YAML::Key << "port" << YAML::Value << comp.port;
            out << YAML::Key << "IPAddress" << YAML::Value << comp.IPAddress;

            out << YAML::EndMap;
        }
        if (entity.HasComponent<ScriptComponentManager>())
        {
            ScriptComponentManager script = entity.GetComponent<ScriptComponentManager>();

            out << YAML::Key << ("ScriptComponentManager");
            out << YAML::BeginMap;

            out << YAML::Key << "count" << YAML::Value << script.Scripts.size();
            out << YAML::EndMap;

            int i = 0; 
            for (ScriptComponent* script : entity.GetComponent<ScriptComponentManager>().GetScripts())
            {
                out << YAML::Key << ("ScriptComponent" + std::to_string(i));
                out << YAML::BeginMap;

                out << YAML::Key << "folderPath" << YAML::Value << script->folderPath.string();
                //out << YAML::Key << "filePath" << YAML::Value << script->filePath;
                out << YAML::Key << "filePath" << YAML::Value << FileSystem::FindRelativeToProject(script->filePath).string();
                out << YAML::Key << "className" << YAML::Value << script->className;

                out << YAML::EndMap;
                i++;
            }
        }

        //if (entity.HasComponent<ScriptComponent>())
        //{
        //    out << YAML::Key << "ScriptComponent";
        //    out << YAML::BeginMap;
        //
        //    auto& script = entity.GetComponent<ScriptComponent>();
        //    out << YAML::Key << "folderPath" << YAML::Value << script.folderPath.string();
        //    out << YAML::Key << "filePath" << YAML::Value << script.filePath;
        //    out << YAML::Key << "className" << YAML::Value << script.className;
        //
        //    out << YAML::EndMap;
        //}
        
        if (entity.HasComponent<AnimationComponent>())
        {
            out << YAML::Key << "AnimationComponent";
            out << YAML::BeginMap;

            auto& anim = entity.GetComponent<AnimationComponent>();
            out << YAML::Key << "count" << YAML::Value << anim.images.size();
            out << YAML::Key << "interval" << YAML::Value << anim.interval;

            out << YAML::EndMap;

            int i = 0;
            for (std::filesystem::path file : anim.images)
            {
                out << YAML::Key << ("AnimFile" + std::to_string(i));
                out << YAML::BeginMap;

                out << YAML::Key << "folderPath" << YAML::Value << FileSystem::FindRelativeToProject(file).string();

                out << YAML::EndMap;
                i++;
            }
        }

        // Add more components here once they are implemented into the engine

        out << YAML::EndMap; // Entity
    }

    void SceneSerialiser::Serialise(const std::string& filePath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene";
        out << YAML::Value << "Untitled Scene";

        out << YAML::Key << "Entities";
        out << YAML::Value << YAML::BeginSeq;
        auto allEntities = m_Scene->m_Registry.view<entt::entity>();
        for (auto entityID : allEntities) {
            Entity currentEntity = { entityID, m_Scene.get() };
            if (!currentEntity) return;
            SerialiseEntity(out, currentEntity);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filePath);
        fout << out.c_str();
    }

    bool SceneSerialiser::Deserialise(const std::string& filePath)
    {
        std::filesystem::path tempFile;

        std::ifstream stream(filePath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data;

        try
        {
            data = YAML::Load(strStream.str());
        }
        catch (YAML::ParserException e)
        {
            return false;
        }

        if (!data["Scene"]) return false;

        std::string sceneName = data["Scene"].as<std::string>();
        LOG_TRACE("Deserializing Scene '{0}'", sceneName);
        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent) {
                    name = tagComponent["Tag"].as<std::string>();
                }
                LOG_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

                Entity deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

                auto relationshipComponent = entity["RelationshipComponent"];
                if (relationshipComponent)
                {
                    auto& rc = deserializedEntity.GetComponent<RelationshipComponent>();
                    rc.parent = relationshipComponent["Parent"].as<uint64_t>();
                }

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();

                    auto& cameraProps = cameraComponent["Camera"];
                    cc.camera.SetProjectionType((ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.camera.SetPerspectiveNear(cameraProps["PerspectiveNear"].as<float>());
                    cc.camera.SetPerspectiveFar(cameraProps["PerspectiveFar"].as<float>());

                    cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.camera.SetOrthographicNear(cameraProps["OrthographicNear"].as<float>());
                    cc.camera.SetOrthographicFar(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }

                auto listenerComponent = entity["AudioListenerComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<AudioListenerComponent>();
                }

                auto AudioComponent = entity["AudioSourceComponent"];
                if (AudioComponent)
                {
                    auto& src = deserializedEntity.AddComponent<AudioSourceComponent>();

                    //glm::vec3 temp = AudioComponent["position"].as<glm::vec3>();
                    //src.position = FMOD_VECTOR{temp.x, temp.y, temp.z};

                    tempFile = FileSystem::FindSystemPath(AudioComponent["soundFile"].as<std::string>());
                    src.soundFile = tempFile.string();
                    src.soundName = tempFile.filename().string();

                    src.volume = AudioComponent["volume"].as<float>();
                    src.pitch = AudioComponent["pitch"].as<float>();
                    src.MaxRange = AudioComponent["MaxRange"].as<float>();
                    src.MinRange = AudioComponent["MinRange"].as<float>();
                    src.loops = AudioComponent["loops"].as<bool>();
                    src.OnAwake = AudioComponent["OnAwake"].as<bool>();


                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if (spriteRendererComponent)
                {
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    src.Colour = spriteRendererComponent["Colour"].as<glm::vec4>();

                    tempFile = FileSystem::FindSystemPath(spriteRendererComponent["Path"].as<std::string>());
                    src.fileName = tempFile.filename().string();
                    src.texture = Project::GetTextureLibrary()->CreateTexture(tempFile.string());

                    src.tilingFactor = spriteRendererComponent["tilingFactor"].as<float>();
                    src.meshType = (SpriteType)spriteRendererComponent["meshType"].as<int>();

                    //src.texture = Texture2D::Create(spriteRendererComponent["Path"].as<std::string>());
                }

                auto circleRender = entity["CircleRendererComponent"];
                if (circleRender)
                {
                    auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
                    src.Colour = circleRender["Colour"].as<glm::vec4>();
                    src.Fade = circleRender["Fade"].as<float>();
                    src.meshType = (SpriteType)circleRender["meshType"].as<int>();
                    src.Radius = circleRender["Radius"].as<float>();
                    src.Thickness = circleRender["Thickness"].as<float>();
                }

                auto rigidbodyComponent = entity["Rigidbody2DComponent"];
                if (rigidbodyComponent)
                {
                    auto& rb = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    rb.fixedRotation = rigidbodyComponent["fixedRotation"].as<bool>();
                    rb.GravityScale = rigidbodyComponent["GravityScale"].as<float>();
                    rb.LinearDamping = rigidbodyComponent["LinearDamping"].as<float>();
                    rb.AngularDamping = rigidbodyComponent["AngularDamping"].as<float>();
                    rb.Mass = rigidbodyComponent["Mass"].as<float>();
                    rb.Type = (Rigidbody2DComponent::BodyType)rigidbodyComponent["Type"].as<int>();
                }

                auto boxColliderComponent = entity["BoxCollider2DComponent"];
                if (boxColliderComponent)
                {
                    auto& box = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                    box.Friction = boxColliderComponent["Friction"].as<float>();
                    //box.Mass = boxColliderComponent["Mass"].as<float>();
                    box.Offset = boxColliderComponent["Offset"].as<glm::vec2>();
                    box.Restitution = boxColliderComponent["Restitution"].as<float>();
                    box.RestitutionThreshold = boxColliderComponent["RestitutionThreshold"].as<float>();
                    box.type = (ColliderTypes)boxColliderComponent["type"].as<int>();
                    box.isTrigger = boxColliderComponent["isTrigger"].as<bool>();
                }

                auto circleColliderComponent = entity["CircleCollider2DComponent"];
                if (circleColliderComponent)
                {
                    auto& circle = deserializedEntity.AddComponent<CircleCollider2DComponent>();
                    circle.Friction = circleColliderComponent["Friction"].as<float>();
                    //circle.Mass = circleColliderComponent["Mass"].as<float>();
                    circle.Size = circleColliderComponent["Size"].as<glm::vec2>();
                    circle.Offset = circleColliderComponent["Offset"].as<glm::vec2>();
                    circle.Radius = circleColliderComponent["Radius"].as<float>();
                    circle.Restitution = circleColliderComponent["Restitution"].as<float>();
                    circle.RestitutionThreshold = circleColliderComponent["RestitutionThreshold"].as<float>();
                    circle.type = (ColliderTypes)circleColliderComponent["type"].as<int>();
                    circle.isTrigger = circleColliderComponent["isTrigger"].as<bool>();

                }

                auto Client = entity["NetworkClientComponent"];
                if (Client)
                {
                    auto& script = deserializedEntity.AddComponent<NetworkClientComponent>();
                    script.port = Client["port"].as<uint16_t>();
                    script.IPAddress = Client["IPAddress"].as<std::string>();

                }
                auto Host = entity["NetworkHostComponent"];
                if (Host)
                {
                    auto& script = deserializedEntity.AddComponent<NetworkHostComponent>();
                    script.port = Host["port"].as<uint16_t>();
                    script.IPAddress = Host["IPAddress"].as<std::string>();

                }
                auto scManager = entity["ScriptComponentManager"];
                if (scManager)
                {
                    int count = scManager["count"].as<int>();

                    for (int i = 0; i < count; i++)
                    {
                        auto currScript = entity["ScriptComponent" + std::to_string(i)];
                        if (currScript) 
                        {
                            ScriptComponent* script = deserializedEntity.GetComponent<ScriptComponentManager>().AddScript();
                            //script->filePath = currScript["filePath"].as<std::string>();
                            script->filePath = FileSystem::FindSystemPath(currScript["filePath"].as<std::string>()).string();

                            script->folderPath = std::filesystem::path(currScript["folderPath"].as<std::string>());


                            script->className = currScript["className"].as<std::string>();
                        }
                    }
                }

                auto anim = entity["AnimationComponent"];
                if (anim)
                {
                    auto& animComp = deserializedEntity.AddComponent<AnimationComponent>();
                    animComp.interval = anim["interval"].as<float>();

                    int count = anim["count"].as<int>();
                    
                    for (int i = 0; i <= count; i++)
                    {
                        auto currAnim = entity["AnimFile" + std::to_string(i)];
                        if (currAnim)
                        {
                            animComp.AddImage(std::filesystem::path(currAnim["folderPath"].as<std::string>()));
                        }
                    }
                }

                //auto currScript = entity["ScriptComponent"];
                //if (currScript)
                //{
                //    ScriptComponent* script = deserializedEntity.GetComponent<ScriptComponentManager>().AddScript();
                //    script->filePath = currScript["filePath"].as<std::string>();
                //    script->folderPath = std::filesystem::path(currScript["folderPath"].as<std::string>());
                //    script->className = currScript["className"].as<std::string>();
                //}

                // Add more components here once they are implemented into the engine
            }
        }
        return true;
    }
}

