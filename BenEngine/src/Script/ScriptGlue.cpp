#pragma once
#include "Engine_PCH.h"
#include "ScriptGlue.h"


#include "Core/UUID.h"
#include "Core/KeyCodes.h"
#include "Core/Input.h"

#include "Mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "Scene/Entity.h"
#include "Physics/PhysicsSystem.h"
#include "Renderer/Animation System/Animator.h"

//for string conversion
#include <locale>
#include <codecvt>

namespace Engine
{
    class Entity;

    namespace Utils {

        static std::string MonoStringToString(MonoString* string)
        {
            if (string == nullptr)
            {
                return "";  // Return empty string or handle the error as needed
            }
            int strLength = mono_string_length(string);
            if (strLength <= 0)
            {
                LOG_ERROR("MonoString is empty or invalid");
                return "";
            }

            char* cStr = mono_string_to_utf8(string);
            if (!cStr)
            {
                LOG_ERROR("Failed to convert MonoString to UTF8");
                return "";
            }

            std::string str(cStr);
            mono_free(cStr); //str takes ownership, no need to free
            return str;
        }

        //template<typename T>
        bool FindScripts(Entity& entity, const char* type)
        {
            if (entity.HasComponent<ScriptComponentManager>())
            {
                for (ScriptComponent* script : entity.GetComponent<ScriptComponentManager>().GetScripts())
                {
                    if (script->className == type)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        
        template<typename T>
        bool FindComponents(Entity& entity, const char* type)
        {
            if (entity.HasComponent<T>())
            {
                T& component = entity.GetComponent<T>();

                if (std::string(typeid(T).name()).find(std::string(type)) != std::string::npos)
                {
                    //LOG_INFO("Component match found!");
                    //LOG_INFO("Entity component: " + std::string(typeid(component).name()));
                    return true;
                }

                // Print or process the component data (for demonstration)
                //LOG_INFO("Processing component: " + std::string(typeid(component).name()));
            }
            return false;
        }

        template<typename... Components>
        void ProcessAllComponents(Entity& entity, const char* type, ComponentGroup<Components...>)
        {
            // Use fold expression to iterate over all types
            if((FindComponents<Components>(entity, type), ...) == false)
                (FindScripts(entity, type));
        }

        template<typename T>
        T* FindAndReturnComponent(Entity& entity, const char* type)
        {
            //do we have the passed component
            if (entity.HasComponent<T>())
            {
                T& component = entity.GetComponent<T>();
                //Does this component contain the same name as the one we are looking for?
                if (std::string(typeid(T).name()).find(std::string(type)) != std::string::npos)
                {
                    //LOG_INFO("Component match found!");
                    //LOG_INFO("Entity component: " + std::string(typeid(component).name()));
                    return &component; // Return a pointer to the component
                }
            }
            return nullptr;
        }
        template<typename T>
        T* FindAndAddComponent(Entity& entity, const char* type)
        {
            //do we have the passed component
            if (!entity.HasComponent<T>())
            {
                //Does this component contain the same name as the one we are looking for?
                if (std::string(typeid(T).name()).find(std::string(type)) != std::string::npos)
                {
                    T& component = entity.AddComponent<T>();

                    //LOG_INFO("Component match found!");
                    //LOG_INFO("Entity component: " + std::string(typeid(component).name()));
                    return &component; // Return a pointer to the component
                }
            }
            return nullptr;
        }

        //Accepts the lists of components
        template<typename... Components>
        void* FindAndReturnComponent(Entity& entity, const char* type, ComponentGroup<Components...>, bool add = false)
        {
            void* result = nullptr;

            // Use a lambda to return the first match, the & indicates it provide a reference
            //result stores the first component pointer that matches the given type.
            ([&] {
                //the result is still not found, so check the next component
                if (result == nullptr)
                {
                    if(!add)
                        result = static_cast<void*>(FindAndReturnComponent<Components>(entity, type));
                    else
                        result = static_cast<void*>(FindAndAddComponent<Components>(entity, type));

                }
                }(), ...);//this ... will call the lamda for each component so we can check if the next type is the correct type

            return result;
        }
        template<typename T>
        static void DeleteArray(T* data) {
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait 5 seconds
            delete[] data;
            std::cout << "Array deleted!" << std::endl;
        }
        template<typename T>
        static void DeleteData(T* data) {
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait 5 seconds
            delete data;
            std::cout << "Data deleted!" << std::endl;
        }

    }



    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define ADD_INTERNAL_CALL(Name) mono_add_internal_call("Braveheart.InternalCalls::" #Name, Name)


    static void NativeLog(MonoString* string, int parameter)
    {

        char* cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        switch (parameter)
        {
        case(0):
            LOG_INFO(str);
            break;
        case(1):
            LOG_WARN(str);
            break;
        case(2):
            LOG_ERROR(str);
            break;
        default:
            break;
        }
    }

    static int NativeGetID() {
        return 505;
    }

    static uint32_t FindEntityByName(MonoString* name)
    {
        Scene* scene = MonoManager::GetSceneContext();
        Entity entity{ scene->GetEntityWithName(Utils::MonoStringToString(name)), scene};


        if (!entity)
            return NULL;

        return (uint32_t)entity;
    } 
    
    static uint32_t CreateNewEntity(MonoString* name, glm::vec3 pos)
    {
        std::string newName = Utils::MonoStringToString(name);

        Scene* scene = MonoManager::GetSceneContext();
        Entity* newEnt = &scene->CreateEntity(newName);
        newEnt->GetComponent<TransformComponent>().Translation = pos;
        return (uint32_t)*newEnt;
    }

    static uint32_t EntityAddComponent(uint32_t id, MonoReflectionType* componentType)
    {
        Scene* scene = MonoManager::GetSceneContext();
        Entity entity{ (entt::entity)id, scene };

        MonoType* managedType = mono_reflection_type_get_type(componentType);

        const char* typeName = mono_type_get_name(managedType);
        if (!typeName)
        {
            LOG_ERROR("Failed to retrieve type name");
            return false;
        }

        void* component = Utils::FindAndReturnComponent(entity, typeName, AllComponents{}, true);
        if (!component)
        {
            LOG_ERROR("Component not found.");
        }
        return id;

    }
    static void Native_DestroyEntity(uint32_t id)
    {
        Scene* scene = MonoManager::GetSceneContext();
        Entity entity{ (entt::entity)id, scene };

        entity.GetScene()->SubmitToDestroyEntity(entity);


    }


    #pragma region Component Handling
        static bool NativeGetComponent(uint32_t id, MonoReflectionType* componentType)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            MonoType* managedType = mono_reflection_type_get_type(componentType);

            const char* typeName = mono_type_get_name(managedType);
            if (!typeName)
            {
                LOG_ERROR("Failed to retrieve type name");
                return false;
            }

            Utils::ProcessAllComponents(entity, typeName, AllComponents{});

            return true;
        }
        static bool NativeHasComponent(uint32_t id, MonoReflectionType* componentType)
        {

            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            MonoType* managedType = mono_reflection_type_get_type(componentType);
            //const char* typeName = mono_type_get_name(managedType);
            char* typeName = mono_type_get_name(managedType);

            if (typeName)
            {
                //LOG_INFO("MonoType Name: " + std::string(typeName));
                void* component = Utils::FindAndReturnComponent(entity, typeName, AllComponents{});

                if (component)
                {
                    //LOG_INFO("Component successfully returned.");
                    return true;
                }
                else
                {
                    LOG_ERROR("Component not found.");
                }
            }
            else
            {
                LOG_ERROR("Failed to retrieve type name");
            }
            mono_free(typeName);
            return false;

        }

        static MonoObject* EntityGetScript(uint32_t id, MonoReflectionType* componentType)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
        
            MonoType* managedType = mono_reflection_type_get_type(componentType);
            const char* typeName = mono_type_get_name(managedType);
            LOG_WARN("SCRIPT NAME : " + std::string(typeName));
            for (ScriptComponent* script : entity.GetComponent<ScriptComponentManager>().GetScripts())
            {
                MonoObject* scriptInstance = script->Script->GetManagedObject();
                if (!scriptInstance)
                    continue;
        
                // Get the MonoClass and MonoType of the script instance
                MonoClass* scriptClass = mono_object_get_class(scriptInstance);
                MonoType* scriptType = mono_class_get_type(scriptClass);
        
                // Get the actual type name
                const char* scriptTypeName = mono_type_get_name(scriptType);
                LOG_WARN("CURR SCRIPT NAME : " + std::string(scriptTypeName));

                if (mono_class_is_subclass_of(scriptClass, MonoManager::EntityClass->m_MonoClass, false) && managedType == scriptType)
                {
                    return scriptInstance;
                }
            }
        
        
            return nullptr;
        }

        static MonoString* TagComponent_GetName(uint32_t id)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            TagComponent& tag = entity.GetComponent<TagComponent>();


            // Directly pass the UTF-8 string to Mono
            MonoString* monoStr = mono_string_new(mono_domain_get(), tag.Tag.c_str());

            return monoStr;
        }

    #pragma endregion

    #pragma region Transform Handling

        //Translation
        static void TransformComponent_GetTranslation(uint32_t id, glm::vec3* outTranslation)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            *outTranslation = transform.Translation;
        }

        static void TransformComponent_SetTranslation(uint32_t id, glm::vec3* translation)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<TransformComponent>())
            {
                entity.GetComponent<TransformComponent>().Translation = *translation; // Assuming Position is the translation
            }
            else
            {
                LOG_ERROR("Entity does not have a TransformComponent!");
            }

        }
        
        static void TransformComponent_GetUp(uint32_t id, glm::vec3* outVector)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<TransformComponent>())
            {
                *outVector = entity.GetComponent<TransformComponent>().GetUp();

            }
            else
            {
                LOG_ERROR("Entity does not have a TransformComponent!");
            }

        }

        //Rotation
        static void TransformComponent_GetRotation(uint32_t id, glm::vec3* outRotation)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            *outRotation = transform.Rotation;
        }

        static void TransformComponent_SetRotation(uint32_t id, glm::vec3* rotation)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<TransformComponent>())
            {
                entity.GetComponent<TransformComponent>().Rotation = glm::radians(*rotation); // Assuming Position is the translation
            }
            else
            {
                LOG_ERROR("Entity does not have a TransformComponent!");
            }

        }

        //Scale
        static void TransformComponent_GetScale(uint32_t id, glm::vec3* outScale)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            *outScale = transform.Rotation;
        }

        static void TransformComponent_SetScale(uint32_t id, glm::vec3* scale)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<TransformComponent>())
            {
                entity.GetComponent<TransformComponent>().Scale = *scale; // Assuming Position is the translation
            }
            else
            {
                LOG_ERROR("Entity does not have a TransformComponent!");
            }

        }

    #pragma endregion

    #pragma region Sprite

        static void SpriteComponent_GetColour(uint32_t id, glm::vec4* outColour)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<SpriteRendererComponent>())
            {
                SpriteRendererComponent& transform = entity.GetComponent<SpriteRendererComponent>();
                *outColour = transform.Colour;
            }
            *outColour = glm::vec4(0.0f);

        }

        static void SpriteComponent_SetColour(uint32_t id, glm::vec4* colour)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };
            if (entity.HasComponent<SpriteRendererComponent>())
            {
                entity.GetComponent<SpriteRendererComponent>().Colour = *colour; // Assuming Position is the translation
            }
            else
            {
                LOG_ERROR("Entity does not have a TransformComponent!");
            }

        }
    
    #pragma endregion

    #pragma region Physics

        //static void Physics2D_RayCast(glm::vec3 origin, glm::vec3 endPoint, RayCast2D::RayCastContext* outhit)
        static void Physics2D_RayCast(glm::vec3 origin, glm::vec3 endPoint, RayCastHit* outhit)
        {
            //RayCast2D::RayCastContext* hit = new RayCast2D::RayCastContext();
            //*hit = PhysicsSystem::RayCast({ origin.x, origin.y }, { endPoint.x, endPoint.y });
            RayCastHit* hit = new RayCastHit();
            *hit = PhysicsSystem::RayCast({ origin.x, origin.y }, { endPoint.x, endPoint.y });

            //if(hit->hit)
            //{   
            //    uint32_t* HitEntity = (PhysicsSystem::)b2Shape_GetUserData(hit));
            //    //if(HitEntity != nullptr)
            //    //{
            //        Scene* scene = MonoManager::GetSceneContext();
            //        Entity ent{ entt::entity((uint32_t)HitEntity), scene };
                    *outhit = *hit;
            //    //}
            //}

        }

        static uint32_t* Physics2D_CircleCast(glm::vec3 origin, float rotation, float radius, int* arraySize)//, uint32_t** outhit)
        {
            std::vector<uint32_t> data = PhysicsSystem::CircleCast({ origin.x, origin.y }, rotation, radius); // Sample values
            *arraySize = data.size();

            uint32_t* outArray = new uint32_t[data.size()];
            std::memcpy(outArray, data.data(), data.size() * sizeof(uint32_t)); // Copy safely

            return outArray;  // freed later in C#

        }

    #pragma endregion

    #pragma region Audio

        static void Native_PlayOneShot(uint32_t id)//, uint32_t** outhit)
        {
            Entity ent{ (entt::entity)id, MonoManager::GetSceneContext() };
            if (ent.HasComponent<AudioSourceComponent>())
            {
                ent.GetComponent<AudioSourceComponent>().PlayOneShot();
            }
        }

    #pragma endregion
    
    #pragma region Rigidbodies
        static void Rigidbody2DComponent_GetLinearVelocity(uint32_t id, glm::vec2* velocity)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2BodyId* body = rb2d.RuntimeBody;
                b2Vec2 vel = b2Body_GetLinearVelocity(*body);
                *velocity = { vel.x, vel.y };
            }
        }
        
        static void Rigidbody2DComponent_GetPosition(uint32_t id, glm::vec3* position)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2BodyId* body = rb2d.RuntimeBody;
                b2Vec2 pos = b2Body_GetPosition(*body);
                *position = { pos.x, pos.y, 0.0f };
            }
        }

        static void Rigidbody2DComponent_SetPosition(uint32_t id, glm::vec3* position)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2BodyId* body = rb2d.RuntimeBody;
                b2Body_SetTransform(*body, { position->x, position->y }, b2Body_GetRotation(*body));
                b2Body_SetAwake(*body, true);
            }
        }

        static void Rigidbody2DComponent_ApplyLinearImpulse(uint32_t id, glm::vec3* impulse, glm::vec3* worldPosition, bool wake)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2BodyId* body = rb2d.RuntimeBody;
                b2Body_ApplyLinearImpulse(*body, b2Vec2{ impulse->x, impulse->y }, b2Vec2{worldPosition->x, worldPosition->y}, wake);
            }
        }
        static void Rigidbody2DComponent_ApplyLinearImpulseToCentre(uint32_t id, glm::vec3* impulse, bool wake)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity { (entt::entity)id, scene };
            if (entity.HasComponent<Rigidbody2DComponent>())
            {
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2BodyId* body = rb2d.RuntimeBody;
                b2Body_ApplyLinearImpulseToCenter(*body, b2Vec2{ impulse->x, impulse->y }, wake);
            }
        }

    #pragma endregion
    
    #pragma region Networking
        static void NetworkClient_SendMessage(uint32_t id, MonoString* string)
        {
            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<NetworkClientComponent>())
            {
                std::string Message = Utils::MonoStringToString(string);

                entity.GetComponent<NetworkClientComponent>().MessageServer(Message);
            }
        }
        
        static void NetworkClient_GetMessages(uint32_t id, MonoString** outString)
        {
            MonoDomain* domain = mono_domain_get(); // Get the current Mono domain
            if (!domain)
            {
                // Handle the case where the domain is not initialized
                return;
            }

            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<NetworkClientComponent>())
            {
                std::string input;

                input = entity.GetComponent<NetworkClientComponent>().connection->latestIncoming;


                *outString = mono_string_new(mono_domain_get(), input.c_str());
            }
        }

        static void NetworkHost_GetMessages(uint32_t id, MonoString** outString)
        {
            MonoDomain* domain = mono_domain_get(); // Get the current Mono domain
            if (!domain)
            {
                // Handle the case where the domain is not initialized
                return;
            }

            Scene* scene = MonoManager::GetSceneContext();
            Entity entity{ (entt::entity)id, scene };

            if (entity.HasComponent<NetworkHostComponent>())
            {
                std::string input;

                input = entity.GetComponent<NetworkHostComponent>().server->latestMessage;


                *outString = mono_string_new(mono_domain_get(), input.c_str());
            }
        }
    #pragma endregion
    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }
    static bool Input_IsMouseDown(MouseCode keycode)
    {
        return Input::IsMouseButtonPressed(keycode);
    }

    void ScriptGlue::RegisterFunctions()
    {
        ADD_INTERNAL_CALL(NativeLog);

        //ADD_INTERNAL_CALL(GetScriptInstance);

        ADD_INTERNAL_CALL(NativeGetID);
        ADD_INTERNAL_CALL(FindEntityByName);
        ADD_INTERNAL_CALL(CreateNewEntity);
        ADD_INTERNAL_CALL(Native_DestroyEntity);
        ADD_INTERNAL_CALL(EntityAddComponent);
        ADD_INTERNAL_CALL(NativeGetComponent);
        ADD_INTERNAL_CALL(NativeHasComponent);
        ADD_INTERNAL_CALL(EntityGetScript);

        ADD_INTERNAL_CALL(TagComponent_GetName);
        ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
        ADD_INTERNAL_CALL(TransformComponent_GetUp);
        ADD_INTERNAL_CALL(TransformComponent_GetRotation);
        ADD_INTERNAL_CALL(TransformComponent_SetRotation);
        ADD_INTERNAL_CALL(TransformComponent_GetScale);
        ADD_INTERNAL_CALL(TransformComponent_SetScale);
        ADD_INTERNAL_CALL(Native_PlayOneShot);

        ADD_INTERNAL_CALL(SpriteComponent_GetColour);
        ADD_INTERNAL_CALL(SpriteComponent_SetColour);

        ADD_INTERNAL_CALL(Physics2D_RayCast);
        ADD_INTERNAL_CALL(Physics2D_CircleCast);
        ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
        ADD_INTERNAL_CALL(Rigidbody2DComponent_GetPosition);
        ADD_INTERNAL_CALL(Rigidbody2DComponent_SetPosition);
        ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
        ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCentre);

        ADD_INTERNAL_CALL(NetworkClient_SendMessage);
        ADD_INTERNAL_CALL(NetworkClient_GetMessages);
        ADD_INTERNAL_CALL(NetworkHost_GetMessages);


        ADD_INTERNAL_CALL(Input_IsKeyDown);
        ADD_INTERNAL_CALL(Input_IsMouseDown);
    }
}