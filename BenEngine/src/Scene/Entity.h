#pragma once
#include "entt.hpp"
#include "Math/Math.h"
#include "Engine.h"

namespace Engine
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle_, Scene* scene_);
        Entity(const Entity& other) = default;

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {   
            CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T>
        T& GetComponent()
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }
        
        template <typename T>
        std::vector<T&> GetComponents()
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        T& GetComponent() const
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent() const
        {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }

        template <typename T>
        void RemoveComponent()
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have the component");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        operator uint32_t() const { return (uint32_t)m_EntityHandle; }
        operator entt::entity() const { return m_EntityHandle; }
        operator bool() const { return m_EntityHandle != entt::null; }

        bool operator == (const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
        bool operator != (const Entity& other) const { return !(*this == other); }

        Scene* GetScene() { return m_Scene; }

        UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
        UUID GetSceneUUID() const;

        Entity GetParent()
        {
            //auto& view = m_Scene->GetAllEntitiesWith<IDComponent>();

            uint32_t id = GetComponent<RelationshipComponent>().parent;
            //
            //auto view = m_Scene->m_Registry.view<IDComponent>();
            //
            //for (auto ent : view)
            //{
            //    Entity otherEnt{ ent, m_Scene };
            //    if (otherEnt.GetComponent<IDComponent>().ID == uuid)
            //        return otherEnt;
            //}

            if (id == entt::null || (uint32_t)id == (uint32_t)m_EntityHandle)
                return Entity{};
            //if (id == entt::null)
            
            //get the parent entity using a entt::handle(uint32_t)
            return Entity{ entt::entity(uint32_t{id}), m_Scene};
        }

        void UpdateToParent();


        //void SetLocal()
        //{
        //    //update children
        //    auto view = m_Scene->m_Registry.view<RelationshipComponent, TransformComponent>();
        //    for (auto e : view)
        //    {
        //        try
        //        {
        //            // Check if the entity has a parent and if the parent is valid
        //            if (GetComponent<RelationshipComponent>().parent != NULL)
        //            {
        //                Entity parent = *GetParent();
        //                if (parent)
        //                {
        //                    std::cout << parent.GetComponent<TransformComponent>().Translation.x << std::endl;
        //                    std::cout << parent.GetComponent<TransformComponent>().Translation.y << std::endl;
        //                    std::cout << parent.GetComponent<TransformComponent>().Translation.z << std::endl;
        //
        //                    //otherEnt.GetComponent<TransformComponent>().Translation += parent.GetComponent<TransformComponent>().Translation;
        //                    // 
        //                    // Get the parent's global transformation matrix
        //                    glm::mat4 parentTransform = parent.GetComponent<TransformComponent>().GetTransform();
        //
        //                    // Get the child's current global position (translation)
        //                    glm::vec3 globalChildPosition = GetComponent<TransformComponent>().Translation;
        //
        //                    // Convert the global child position to local space by applying the inverse of the parent's transform
        //                    glm::vec3 localChildPosition = glm::inverse(parentTransform) * glm::vec4(globalChildPosition, 1.0f);
        //
        //                    // Now you can modify the local child position
        //                    localChildPosition += glm::vec3(1.0f, 0.0f, 0.0f);  // Move the child by some local offset
        //
        //                    // Convert back to global space
        //                    GetComponent<TransformComponent>().Translation = parentTransform * glm::vec4(localChildPosition, 1.0f);
        //
        //                }
        //            }
        //        }
        //        catch (...)
        //        {
        //            //something went wrong with components
        //        }
        //
        //    }
        //
        //
        //}


    private:
        entt::entity m_EntityHandle = entt::null;
        Scene* m_Scene = nullptr;

        friend class Scene;
        friend class SceneSerialiser;
    };
}
