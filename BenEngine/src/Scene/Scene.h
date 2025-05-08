#pragma once
#include "Engine.h"
#include "entt.hpp"
#include "Core/UUID.h"
#include <typeindex>
#include "Renderer/Camera/EditorCamera.h"
#include <Box2D/box2d.h>
#include "Math/Math.h"


namespace Engine
{
	class Entity;
	class NetworkManager;

	struct CopiedComponent
	{
		std::type_index Type;
		std::function<void(Entity&)> PasteFunction;

		CopiedComponent() : Type(typeid(void)) {};
	};

	class Scene
	{
	public:
		Scene(const std::string& name = "Untitled Scene", bool isEditorScene = false);
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);
		static void Scene::CopyTo(Ref<Scene> source, Ref<Scene> destination);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");

		std::map<UUID*, Entity*> AllEntities;

		Entity DuplicateEntity(Entity entity);

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);

		//instead of just one update, two updates are now used. These will switch depending on if the game engine is in
		//editor mode or runtime
		//void OnUpdate(Timestep ts);

		void DestroyEntity(Entity ent);
		void OnViewportResize(uint32_t width, uint32_t height);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void SubmitToDestroyEntity(Entity entity);

		template<typename T>
		void RegisterComponentHandler(std::function<void(Entity, T&)> handler)
		{
			m_ComponentHandlers[typeid(T)] = [handler](Entity entity) {
				handler(entity, entity.GetComponent<T>());
				};
		}

		template<typename Fn>
		void SubmitPostUpdateFunc(Fn&& func)
		{
			m_PostUpdateQueue.emplace_back(func);
		}

		template<typename T>
		void OnComponentAdded(Entity entity, T& component)
		{
			auto it = m_ComponentHandlers.find(typeid(T));
			if (it != m_ComponentHandlers.end())
			{
				it->second(entity);
			}
			else
			{
				LOG_INFO("Component of type {0} added to Entity ID {1}", typeid(T).name(), (uint32_t)entity);
			}
		}


		template<typename ... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}


		entt::entity GetEntityWithName(std::string name)
		{
			auto& view = m_Registry.view<TagComponent>();
			for (auto& ent : view)
			{
				TagComponent t = m_Registry.get<TagComponent>(ent);

				if (t.Tag == name)
				{
					LOG_INFO("Found entity " + t.Tag);
					return ent;
				}
			}
		}


		const std::string& GetName() const { return m_Name; };
		void SetName(const std::string& name) { m_Name = name; };

		template<typename TComponent>
		void CopyComponentIfExists(entt::entity dst, entt::registry& dstRegistry, entt::entity src)
		{
			if (m_Registry.any_of<TComponent>(src))
			{
				auto& srcComponent = m_Registry.get<TComponent>(src);
				dstRegistry.emplace_or_replace<TComponent>(dst, srcComponent);
			}
		}

		template<typename T>
		static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			auto srcEntities = srcRegistry.view<T>();
			for (auto srcEntity : srcEntities)
			{
				entt::entity destEnt = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = srcRegistry.get<T>(srcEntity);	
				auto& destComponent = dstRegistry.emplace_or_replace<T>(destEnt, srcComponent);
			}
		}

		template<typename T>
		void CopyComponent(Entity entity)
		{
			if (!entity.HasComponent<T>())
				return;

			auto& component = entity.GetComponent<T>();
			m_CopiedComponent.Type = typeid(T);
			m_CopiedComponent.PasteFunction = [component](Entity& targetEntity) mutable {
				if (!targetEntity.HasComponent<T>())
					targetEntity.AddComponent<T>(component); // Add the copied component to the target entity
				else
					targetEntity.GetComponent<T>() = component; // Update if already exists
				};
		}


		void CopyRelationships(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap);
		


		void PasteComponent(Entity& targetEntity)
		{
			if (m_CopiedComponent.Type != typeid(void) && m_CopiedComponent.PasteFunction)
			{
				m_CopiedComponent.PasteFunction(targetEntity);
				m_CopiedComponent.Type = typeid(void);
			}
		}

		bool HasCopiedComponent() const
		{
			return m_CopiedComponent.Type != typeid(void);
		}


	private:
		uint32_t GetViewportWidth() { return m_ViewportWidth; }
		uint32_t GetViewportHeight() { return m_ViewportHeight; }

		void SetSceneID(UUID id) { m_SceneID = id; }
		UUID GetSceneID() { return m_SceneID; }

	private:
		UUID m_SceneID;

		entt::registry m_Registry;

		Ref<SoundSystem> RuntimeSound;

		b2WorldId m_Physics2DWorld;
		NetworkManager* NetworkWorld;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class ContentPanel;
		friend class SceneSerialiser;
		friend class EditorLayer;
		friend class PhysicsSystem;
		friend class NetworkManager;

		std::unordered_map<std::type_index, std::function<void(Entity)>> m_ComponentHandlers;
		std::vector<std::function<void()>> m_PostUpdateQueue;


		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_Name;
		bool m_IsEditorScene = false;	

		CopiedComponent m_CopiedComponent;

	public:
		bool isRunning = false;
	};
}