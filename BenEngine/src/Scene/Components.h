#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <format>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "Core/UUID.h"
#include "Utils/PlatformUtils.h"
#include "SceneCamera.h"

#include <filesystem>

#include "Box2D/box2d.h"
#include <Box2D/src/body.h>
#include "Editor/Project.h"
#include "Script/MonoManager.h"

#include "Network/NetworkClient.h"
#include "Network/NetworkHost.h"
//#include "Engine.h"

#include "Physics/PhysicsSystem.h";
#include "Renderer/Animation System/Animator.h"
#include <Logging/Log.h>
#include <AI/NavMesh.h>
#include <AI/AIAgent.h>


namespace Engine
{
    //class Entity;

    //when an ID component is made, it has a base ID of 0 but this is changed by default
    //the default will create the UUID class that produces a random ID.
    struct IDComponent
    {
        UUID ID = 0;

        IDComponent() = default; //allows the creation of an ID component without manual initialisation of its members
        IDComponent(const IDComponent&) = default; //allows the copying of an ID component from another instance
    };

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag_) : Tag(tag_) {}
    };

    struct RelationshipComponent
    {
        //Entity* parent = nullptr;
        //UUID parent = NULL;
        uint32_t parent = NULL;

        RelationshipComponent() = default;
        RelationshipComponent(const RelationshipComponent&) = default;
        //RelationshipComponent(Entity* ent) : parent(ent) {}


    };


    struct TransformComponent
    {
        glm::mat4 Transform{ 1.0f };

        glm::vec3 Translation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

        glm::vec3 WorldPos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 WorldRot = { 0.0f, 0.0f, 0.0f };
        glm::vec3 WorldScale = { 0.0f, 0.0f, 0.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation) : Translation(translation) {}

        glm::mat4 GetLocalTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation) * rotation *
                glm::scale(glm::mat4(1.0f), Scale);
        }

        glm::mat4 GetTransform() const
        {
            glm::vec3 TempPos = Translation + WorldPos;
            glm::vec3 TempScale = Scale + WorldScale;
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation + WorldRot));


            return glm::translate(glm::mat4(1.0f), TempPos) * rotation *
                glm::scale(glm::mat4(1.0f), TempScale);
        }


        glm::quat GetOrientation() const
        {
            //return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
            return glm::quat(glm::vec3(-Rotation.y, -Rotation.x, 0.0f));
        }
        
        glm::vec3 GetUp() const
        {
            // The canonical world up vector
            glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

            // Retrieve the full transform matrix (this combines translation, rotation, and scale)
            glm::mat4 transform = GetTransform();

            // Extract the up vector from the transformed space
            glm::vec3 relativeUp = glm::vec3(transform * glm::vec4(worldUp, 0.0f));

            return relativeUp; // Normalize to ensure it's a unit vector
        }

        
    };

    struct CameraComponent
    {
        SceneCamera camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        //default will create the class with automated variables

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(ProjectionType t)
        {
            camera.SetProjectionType(t);
        }

        operator SceneCamera& () { return camera; }
        operator const SceneCamera& () const { return camera; }
    };

    enum SpriteType 
    {
        Square,
        Triangle,
        Circle
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
        SpriteType meshType = Square;

        std::string fileName;
        Ref<Texture2D> texture;
        float tilingFactor = 1.0f;

        //SpriteRendererComponent() = default;
        SpriteRendererComponent() { texture = Project::GetTextureLibrary()->CreateTexture("src/Renderer/BasicTextures/WhiteTexture.png"); }
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4 colour) : Colour(colour) {}
    };

    struct CircleRendererComponent
    {
        glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };

        float Radius = 0.5f;
        float Thickness = 1.0f;
        float Fade = 0.005f;
            
        SpriteType meshType = Circle;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
        CircleRendererComponent(const glm::vec4 colour) : Colour(colour) {}
    };

    //physics components

    struct Rigidbody2DComponent
    {
        static enum class BodyType { Static = 0, Kinematic = 1, Dynamic = 2, };

        BodyType Type = BodyType::Dynamic;
        bool fixedRotation = false;
        float GravityScale = 1.0f;
        float Mass = 1.0f;
        float LinearDamping = 1.0f;
        float AngularDamping = 0.0f;

        b2BodyId* RuntimeBody = nullptr;
        b2ShapeId* RuntimeShape = nullptr;
            
        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
        Rigidbody2DComponent(BodyType type) : Type(type) {}



        BodyType GetType()
        {
            return Type;
        }




        void ContactBegin(b2ShapeId A, b2ShapeId B)
        {
            printf(A.index1 + " is touching " + B.index1);
        }

    };

    enum class ColliderTypes { Box, Circle, Capsule };

    struct Collider2DComponent;

    struct BoxCollider2DComponent
    {
        bool isTrigger = false;

        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };  // Default Box size
    
        //float Mass = 1.0f;
        float Mass = 0.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f; //bounciness
        float RestitutionThreshold = 0.5f;
    
        void* RuntimeFixture = nullptr;
    
        ColliderTypes type = ColliderTypes::Box;
    
        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    
        //void Build(b2BodyId* body, glm::vec3 scaleModifier, entt::entity handle)
        b2ShapeId Build(b2BodyId* body, glm::vec3 scaleModifier, uint32_t handle)
        {
            b2Polygon dynamicBox = b2MakeBox(Size.x * scaleModifier.x, Size.y * scaleModifier.y);
    
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = Mass;
            shapeDef.friction = Friction;
            shapeDef.restitution = Restitution;

            shapeDef.isSensor = isTrigger;

            shapeDef.enableContactEvents = true;

            PhysicsSystem::ShapeData* shape = new PhysicsSystem::ShapeData{(uint32_t)handle, false};
            shapeDef.userData = reinterpret_cast<void*>((PhysicsSystem::ShapeData*)shape);
            //shapeDef.userData = (&handle);

            b2ShapeId shapeId = b2CreatePolygonShape(*body, &shapeDef, &dynamicBox);

            return shapeId;
        }
    };

    struct CircleCollider2DComponent
    {
        bool isTrigger = false;

        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };  // Default Box size
        float Radius = 0.5f;

        float Mass = 0.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f; //bounciness
        float RestitutionThreshold = 0.5f;

        void* RuntimeFixture = nullptr;

        ColliderTypes type = ColliderTypes::Circle;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;

        b2ShapeId* Build(b2BodyId* body, glm::vec3 scaleModifier, uint32_t handle)
        {
            b2Circle circle = b2Circle();
            circle.center = b2Vec2{ 0.0f, 0.0f };
            circle.radius = Radius;

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = Mass;
            shapeDef.friction = Friction;
            shapeDef.restitution = Restitution;
            shapeDef.isSensor = isTrigger;

            //shapeDef.userData = reinterpret_cast<void*>(handle);
            PhysicsSystem::ShapeData* shape = new PhysicsSystem::ShapeData{ (uint32_t)handle, false };
            shapeDef.userData = reinterpret_cast<void*>((PhysicsSystem::ShapeData*)shape);

            b2ShapeId* shapeId = &b2CreateCircleShape(*body, &shapeDef, &circle);

            return shapeId;

        }

    };


    struct Collider2DComponent
    {
        ColliderTypes type = ColliderTypes::Box;

        std::unique_ptr<Collider2DComponent> comp;

        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };  // Default Box size

        float Mass = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f; //bounciness
        float RestitutionThreshold = 0.5f;

        void* RuntimeFixture = nullptr;

        Collider2DComponent() = default;
        Collider2DComponent(ColliderTypes type)
        {
            UpdateCollider(type);
        }
        Collider2DComponent(const Collider2DComponent&) = default;

        void UpdateCollider(ColliderTypes type)
        {
            if (type == ColliderTypes::Box)
            {
                //comp = std::unique_ptr<Collider2DComponent>(std::make_unique<BoxCollider2DComponent>());

            }
        }

        virtual void Build(b2BodyId* body, glm::vec3 scaleModifier) {};

        void BeginBuild(b2BodyId* body, glm::vec3 scaleModifier)
        {
            comp->Build(body, scaleModifier);
        }
    };

    struct AnimationComponent
    {
        std::vector<std::filesystem::path> images;
        std::vector<Ref<SubTexture2D>> sprites;
        Animator* anim = nullptr;

        int index = 0;
        float timeElapsed = 0.0f;
        float interval = 0.1f;
        bool playing = false;

        glm::vec2 spriteCellTable = {1, 1};
        glm::vec2 spriteCellSize = {100, 100};

        AnimationComponent() = default;
        AnimationComponent(const AnimationComponent&) = default;
        ~AnimationComponent() = default;

        void AddImage(std::filesystem::path image)
        {
            images.emplace_back(image);
        }
        void RemoveImage(int it)
        {
            images.erase(images.begin() + it);
        }


        void Play()
        {
            anim = new Animator(images, sprites, interval);
            anim->Play();
        }

        void Update(float ts, uint32_t ent, Scene* scene)
        {
            anim->Update(ts, ent, scene);
        }

        void Stop() { anim->Stop(); }

    };


    struct AudioListenerComponent
    {
        FMOD_VECTOR lastPos = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR listenerPos = lastPos;

        // Calculate velocity
        FMOD_VECTOR listenerVel = {
            listenerPos.x - lastPos.x,
            listenerPos.y - lastPos.y,
            listenerPos.z - lastPos.z
        };

        FMOD_VECTOR listenerForward = { 0.0f, 0.0f, -1.0f };
        FMOD_VECTOR listenerUp = { 0.0f, 1.0f, 0.0f };

        float MaxRange = 10.0f;
        float MinRange = 0.0f;

        AudioListenerComponent() = default;
        AudioListenerComponent(const AudioListenerComponent&) = default;

        void ListenerUpdate(FMOD::System* system)
        {
            // Calculate velocity based on change in position
            listenerVel.x = listenerPos.x - lastPos.x;
            listenerVel.y = listenerPos.y - lastPos.y;
            listenerVel.z = listenerPos.z - lastPos.z;

            // Update FMOD listener
            system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);

            lastPos = listenerPos;

            //system->update();
        }

        void UpdatePos(glm::vec3 pos)
        {
            listenerPos = FMOD_VECTOR{
                pos.x,
                pos.y,
                pos.z };
        }

        void SetSoundRange(FMOD::Sound* sound, float minDistance, float maxDistance)
        {
            sound->set3DMinMaxDistance(minDistance, maxDistance);
        }


    };

    struct AudioSourceComponent
    {

        //3D variables for 3D sound sources
        FMOD_VECTOR oldPos = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR position = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };
        FMOD_VECTOR velocity = FMOD_VECTOR{ 0.0f, 0.0f, 0.0f };

        std::string soundName = "NO FILE SELECTED";
        std::string soundFile = "0";
        FMOD::Sound* sound = nullptr;
        FMOD::System* m_SoundSystem = nullptr;

        bool loops = false;
        bool OnAwake = false;

        float volume = 0.5f; //max is 1, min is 0 (0% - 100%)
        float pitch = 1.0f; //1.0f is normal, 0.5f is lower, 2.0f is higher

        float MaxRange = 100.0f;
        float MinRange = 0.0f;

        FMOD::Channel* channel;

        AudioSourceComponent() = default;
        AudioSourceComponent(const AudioSourceComponent&) = default;
        ~AudioSourceComponent()
        {
            Stop();
            if (sound)
            {
                sound->release();
                sound = nullptr;
            }
            if (channel)
            {
                channel = nullptr;
            }
        }


        void Pause()
        {
            if (channel)
                channel->setPaused(true);
        }
        void Resume()
        {
            if (channel)
                channel->setPaused(false);
        }
        void Stop()
        {
            if (channel)
            {
                channel->stop();
            }

        }

        //update the position of the sound for audio sources
        void UpdatePos(glm::vec3 pos)
        {
            FMOD_VECTOR oldPos = position;

            position = FMOD_VECTOR{ pos.x, pos.y, pos.z };

            // Calculate velocity (change in position)
            velocity.x = position.x - oldPos.x;
            velocity.y = position.y - oldPos.y;
            velocity.z = position.z - oldPos.z;

        }

        //create a reference the curront fmod system
        void LoadSystem(FMOD::System* system)
        {
            m_SoundSystem = system;
        }

        //Creat a sound from a file into the fmod system for playing
        void LoadSound(FMOD::System* system)
        {
            //setting modes are important for getting the correct 3D environment effects
            
            //system->createSound(soundFile.c_str(), FMOD_DEFAULT, nullptr, &sound);
            if(loops)
                system->createSound((std::filesystem::current_path() / soundFile).string().c_str(), FMOD_LOOP_NORMAL | FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &sound);
            else
                system->createSound((std::filesystem::current_path() / soundFile).string().c_str(), FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &sound);

            sound->setMode(FMOD_3D_LINEARROLLOFF);

            sound->set3DMinMaxDistance(MinRange, MaxRange);

            m_SoundSystem = system;
        } 

        //Play a sound - this will be the main sound that is produced, only one of these can be playing
        void Play(std::string file = "")
        {
            if (m_SoundSystem == nullptr)
            {
                LOG_ERROR("NO SOUND SYSTEM HAS BEEN LOADED");
                return;
            }

            if (file == "")
                file = soundFile;

            else
                soundFile = file;


            if (soundFile == "")
            {
                LOG_ERROR("NO AUDIO HAS BEEN LOADED");
                return;
            }

            soundFile = (std::filesystem::current_path().parent_path().parent_path() / soundFile).string();

            Stop();
            LoadSound(m_SoundSystem);
            if(sound != nullptr)
                PlaySound(m_SoundSystem);
        }


        //plays a one shot of an audio file, not interuppting the existing sound.
        void PlayOneShot(std::string file = "")
        {
            if (file == "")
                file = (std::filesystem::current_path().parent_path().parent_path() / soundFile).string();

            std::thread PlayThread([this, file]() { OneShot(file); });
            PlayThread.detach();
        }
        void OneShot(std::string file = "")
        {
            if (m_SoundSystem == nullptr)
            {
                LOG_ERROR("NO SOUND SYSTEM HAS BEEN LOADED");
                return;
            }

            FMOD::Sound* OneShot = nullptr;
            FMOD::Channel* tempChannel;

            if (file == "")
            {
                LOG_ERROR("NO AUDIO HAS BEEN LOADED");
                return;
            }

            // Create a sound with FMOD_CREATESAMPLE for a one-shot effect
            m_SoundSystem->createStream(file.c_str(), FMOD_3D | FMOD_CREATESAMPLE, nullptr, &OneShot);
            //m_SoundSystem->createStream
            OneShot->setMode(FMOD_3D_LINEARROLLOFF);

            OneShot->set3DMinMaxDistance(MinRange, MaxRange);

            // Play the sound on a free channel 
            m_SoundSystem->playSound(OneShot, nullptr, false, &tempChannel);

            tempChannel->setMode(FMOD_3D | FMOD_3D_LINEARROLLOFF);

            tempChannel->set3DSpread(360.0f); // Wider sound spread

            tempChannel->set3DAttributes(&position, &velocity);

            // Set volume to 50%
            tempChannel->setVolume(volume);

            // Set pitch (1.0f is normal, 0.5f is lower, 2.0f is higher)
            tempChannel->setPitch(pitch);

            bool isPlaying = true;
            while(isPlaying == true)
            {
                m_SoundSystem->update();
                tempChannel->isPlaying(&isPlaying);
            }
            tempChannel->stop();
            OneShot->release();

        }


        void PlaySound(FMOD::System* system = nullptr)
        {
            free(channel);
            m_SoundSystem = system == nullptr ? m_SoundSystem : system;

            m_SoundSystem->playSound(sound, nullptr, false, &channel);

            if (channel)
            {
                channel->setMode(FMOD_3D | FMOD_3D_LINEARROLLOFF);

                channel->set3DSpread(360.0f); // Wider sound spread

                channel->set3DAttributes(&position, &velocity);

                // Set volume to 50%
                channel->setVolume(volume);

                // Set pitch (1.0f is normal, 0.5f is lower, 2.0f is higher)
                channel->setPitch(pitch);

                // Check if sound is playing
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);

                if (!isPlaying)
                {
                    std::cout << "Sound finished playing!" << std::endl;
                }
            }
        }


    };

    //Network system
    struct NetworkHostComponent
    {
        std::string IPAddress = "127.0.0.1";
        uint16_t port = 5046;
        NetworkHost* server;

        NetworkHostComponent() = default;
        NetworkHostComponent(const NetworkHostComponent&) = default;
    };
    struct NetworkClientComponent
    {
        std::string IPAddress = "127.0.0.1";
        uint16_t port = 5046;

        int localID = 0;
        int globalID = 0;
        bool isLocal;
        NetworkClient* connection;

        NetworkClientComponent() = default;
        NetworkClientComponent(const NetworkClientComponent&) = default;

        void MessageServer(std::string data) { connection->MessageServer(data); }
        void CheckRecieving() { connection->ReadData(); }
    };

    struct ScriptComponent
    {

        std::string className = "NONE";
        uint32_t Owner;

        std::filesystem::path folderPath = Project::currCodeProj / "Assets/Scripts";
        std::string filePath = "NO FILE";  // script file

        Ref<ScriptInstance> Script = nullptr;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;

        void AssignScript(std::filesystem::path ScriptPath)
        {
            filePath = ScriptPath.string();
            className = ScriptPath.filename().stem().string();
            MonoManager::CompileScripts();

        }

        void AssignScriptClass(Ref<ScriptInstance> script)
        {
            Script = script;
        }

        void CreateScript(std::string newName)
        {
            className = newName;

            filePath = (folderPath / (className + ".cs")).string();
            // Ensure the folder exists
            if (!std::filesystem::exists(folderPath)) {
                if (std::filesystem::create_directories(folderPath)) {
                    std::cout << "Class created: " << folderPath << std::endl;
                }
                else {
                    std::cerr << "Failed to create class!" << std::endl;
                }
            }

            // Create and write to the file
            std::string scriptTemplate = 
(R"(using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class [CLASSNAME] : GameObject
    {
        public [CLASS]() : base ()
        {

        }

        public void Start()
        {
            Debug.Log("Start");
        }

        public void Update(float delta)
        {
            Debug.Log("Update");
        }
    }
}

)");
            // Replace [NAME]
            size_t name = scriptTemplate.find("[CLASSNAME]");
            if (name != std::string::npos) {
                scriptTemplate.replace(name, 11, className);
            }
            size_t constructor = scriptTemplate.find("[CLASS]");
            if (constructor != std::string::npos) {
                scriptTemplate.replace(constructor, 7, className);
            }

            //std::ofstream outFile(filePath);
            //if (outFile) {
            //    outFile << scriptTemplate << std::endl;
            //    std::cout << "File created successfully: " << filePath << std::endl;
            //    className = std::filesystem::path(filePath).filename().string();
            //
            //}
            //else {
            //    std::cerr << "Failed to create file!" << std::endl;
            //}
            filePath = FileSystem::CreateNewFile(folderPath, className + ".cs", scriptTemplate).string();
            if (std::filesystem::exists(std::filesystem::path(filePath)))
            {
                className = std::filesystem::path(filePath).filename().stem().string();
            }

        }
    
    };




    struct ScriptComponentManager
    {
        std::vector<ScriptComponent*> Scripts;


        ScriptComponentManager() = default;
        ScriptComponentManager(const ScriptComponentManager&) = default;
        ~ScriptComponentManager()
        { 
            //LOG_INFO("DESTROYING THIS SCRIPT"); 
            //for (ScriptComponent* script : Scripts)
            //{
            //    MonoObject* scriptInstance = script->Script->GetManagedObject();
            //    if (scriptInstance)
            //    {
            //        script->Script->SetMangedObject(nullptr); // Nullify the instance
            //    }
            //}
            //
            //// Remove script components
            //Scripts.clear();

        }


        ScriptComponent* AddScript()
        {
            ScriptComponent* newScript = new ScriptComponent();
            Scripts.push_back(newScript);
            return newScript;
        }
        std::vector<ScriptComponent*>& GetScripts() { return Scripts; }

        ScriptComponent* FindScript(std::string className)
        {
            for (ScriptComponent* script : Scripts)
            {
                if (script->className == className)
                    return script;
            }
            return nullptr;
        }



    };


    struct NavMeshComponent
    {
        NavMesh* mesh;

        NavMeshComponent() = default;
        NavMeshComponent(const NavMeshComponent&) = default;

        void BeginGeneration()
        {
            if (mesh == nullptr)
            {
                mesh = new NavMesh();
            }

            mesh->Generate();
        }

    };

    struct AINavigatorComponent
    {
        std::vector<glm::vec3*> points;
        bool moving = true;
        AIAgent* agent;

        AINavigatorComponent() = default;
        AINavigatorComponent(const AINavigatorComponent&) = default;

        void RuntimeStart(b2BodyId* id, NavMesh* navData)
        {
            if (agent == nullptr)
                agent = new AIAgent(id, navData->GetNavMesh());
        }

    };

    template<typename... Component>
    struct ComponentGroup
    {
    };

    using AllComponents =
        ComponentGroup<TagComponent, TransformComponent, IDComponent, RelationshipComponent, SpriteRendererComponent,
        CircleRendererComponent, CameraComponent,
        Rigidbody2DComponent, BoxCollider2DComponent,
        CircleCollider2DComponent, AudioListenerComponent, AnimationComponent, AudioSourceComponent, NetworkHostComponent, NetworkClientComponent, ScriptComponent>;
}
