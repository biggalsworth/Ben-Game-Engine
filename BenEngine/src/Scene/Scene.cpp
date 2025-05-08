#include "Engine_PCH.h"
#include "Scene.h"
#include "Physics/PhysicsSystem.h"
#include "Entity.h"
#include "Components.h"
#include "Network/NetworkManager.h"

namespace Engine
{
	Scene::Scene(const std::string& name, bool isEditorScene) : m_Name(name), m_IsEditorScene(isEditorScene)
	{
		RegisterComponentHandler<CameraComponent>([](Entity entity, CameraComponent& component) {
			LOG_INFO("Camera component added");
			if (auto* scenePtr = entity.GetScene())
			{
				component.camera.SetViewportSize(scenePtr->GetViewportWidth(), scenePtr->GetViewportHeight());
			}
			});


	}

	Scene::~Scene()
	{

	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		CopyTo(other, newScene);
		return newScene;
	}

	void Scene::CopyTo(Ref<Scene> source, Ref<Scene> destination)
	{
		if (destination == nullptr) destination = CreateRef<Scene>();

		destination->m_ViewportHeight = source->m_ViewportHeight;
		destination->m_ViewportWidth = source->m_ViewportWidth;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneReg = source->m_Registry;
		auto& dstSceneReg = destination->m_Registry;

		auto idView = srcSceneReg.view<IDComponent>();


		for (auto e : idView)
		{
			Entity ent = Entity{ e, source.get()};

			LOG_INFO("ENT NAME: " + ent.GetComponent<TagComponent>().Tag);
			LOG_INFO("ENT ID: " + std::to_string((uint32_t)ent));

			UUID uuid = srcSceneReg.get<IDComponent>(e).ID;

			LOG_INFO(std::string("UUID FOR " + ent.GetComponent<TagComponent>().Tag + " BEFORE: " +  std::to_string(uuid)));

			const auto& tag = srcSceneReg.get<TagComponent>(e).Tag;
			Entity en = destination->CreateEntityWithID(uuid, tag);

			LOG_INFO(std::string("UUID FOR " + en.GetComponent<TagComponent>().Tag + " AFTER: " + std::to_string(uuid)));
			LOG_INFO("ENT ID: " + std::to_string((uint32_t)en));

			enttMap[uuid] = en.m_EntityHandle;
			en.GetComponent<ScriptComponentManager>() = srcSceneReg.get<ScriptComponentManager>(e);
		}

		CopyComponent<TagComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<RelationshipComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<TransformComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<CameraComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<AudioListenerComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<AudioSourceComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<NetworkClientComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<NetworkHostComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<AnimationComponent>(dstSceneReg, srcSceneReg, enttMap);

		CopyComponent<NavMeshComponent>(dstSceneReg, srcSceneReg, enttMap);
		CopyComponent<AINavigatorComponent>(dstSceneReg, srcSceneReg, enttMap);
		//CopyComponent<ScriptComponent>(dstSceneReg, srcSceneReg, enttMap);

		// Step 3: Copy Relationships (if any)
		// Ensure that relationships are maintained by copying parent/child entities
		destination->CopyRelationships(dstSceneReg, srcSceneReg, enttMap);
		destination->m_IsEditorScene = false;

	}

	Entity Scene::CreateEntity(const std::string& name)
	{

		return CreateEntityWithID(UUID(), name);

		//not used as the other function can create this data while using randomly generated UUIDs
		/*
		
		//both these lines are the same, they create the entity and create a new entity handle
		//for the registry to handle and pass this scene as the attatched scene
		//   \/ \/ \/ \/ \/
		//Entity entity = Entity( m_Registry.create(), this );
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name; //if no name is given then use a default tag of "Entity"
		
		return entity;

		*/
	

	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		
		//both these lines are the same, they create the entity and create a new entity handle
		//for the registry to handle and pass this scene as the attatched scene
		//   \/ \/ \/ \/ \/
		//Entity entity = Entity( m_Registry.create(), this );
		auto entity = Entity{ m_Registry.create(), this };

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid); //assigns the variable in brackets to the variable within the component (uuid goes to IDComponent::ID)
		
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name; //if no name is given then use a default tag of "Entity"

		entity.AddComponent<RelationshipComponent>().parent = (uint32_t)entt::null;

		entity.AddComponent<ScriptComponentManager>();


		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity;
		if (entity.HasComponent<TagComponent>())
		{
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		}
		else 
		{
			newEntity = CreateEntity();
		}

		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<RelationshipComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<NetworkClientComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<NetworkHostComponent>(newEntity.m_EntityHandle, m_Registry, entity);

		//CopyComponentIfExists<ScriptComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<ScriptComponentManager>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<AnimationComponent>(newEntity.m_EntityHandle, m_Registry, entity);

		CopyComponentIfExists<NavMeshComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<AINavigatorComponent>(newEntity.m_EntityHandle, m_Registry, entity);


		return newEntity;
	}

	void Scene::DestroyEntity(Entity ent)
	{
		//remove any child linked to this object
		auto view = GetAllEntitiesWith<RelationshipComponent, TransformComponent>();
		for (auto e : view)
		{
			Entity otherEnt{ e, ent.m_Scene };
			// Check if the entity has a parent and if the parent is valid
			if (otherEnt.HasComponent<RelationshipComponent>() && otherEnt.GetComponent<RelationshipComponent>().parent != NULL)
			{
				//if (otherEnt.GetComponent<RelationshipComponent>().parent == (uint32_t)ent)
				auto p = otherEnt.GetParent();
				if ((uint32_t)otherEnt.GetParent() == (uint32_t)ent)
				{
					DestroyEntity(otherEnt);
				}
			}
		}
		if (ent.HasComponent<Rigidbody2DComponent>() && isRunning)
			PhysicsSystem::DestroyBody(ent);

		m_Registry.destroy(ent);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		////update children
		//auto view = GetAllEntitiesWith<RelationshipComponent, TransformComponent>();
		//for (auto e : view)
		//{
		//	Entity otherEnt{ e, this };
		//	otherEnt.UpdateToParent();
		//}

		Renderer2D::BeginScene(camera);

		//Draw sprites	
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}
		//Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Colour, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		//Draw colliders
		{
			{
				auto view = m_Registry.view<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transform, collider] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					Renderer2D::DrawCollider(transform.GetTransform(), collider, (int)entity);
				}
			}
			{
				auto view = m_Registry.view<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transform, collider] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
					glm::vec3 pos = transform.Translation + glm::vec3(collider.Offset, 0.001f);
					glm::vec3 scale = transform.Scale * glm::vec3(collider.Radius * 2.0f);
					glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1), scale);

					Renderer2D::DrawColliderCircle(newTransform, collider, (int)entity);
				}
			}
		}
		Renderer2D::EndScene();
	}

	void Scene::OnRuntimeStart()
	{
		//this is where we will eventually:
		//1. handle physics
		//2. setup colliders
		//3. setup rigidbodies

		PhysicsSystem::SetWorld(this, &m_Registry);
		PhysicsSystem::StartWorld();

		//Setup a network
		NetworkWorld = new NetworkManager(this);
		NetworkWorld->SetupNetwork();


		//Begin mono manager
		MonoManager::RuntimeStart(this);


		auto& scriptComponents = GetAllEntitiesWith<ScriptComponentManager>();
		for (auto& comp : scriptComponents)
		{
			Entity ent{ comp, this };
			LOG_INFO("ENT NAME: " + ent.GetComponent<TagComponent>().Tag);
			LOG_INFO("ENT ID: " + std::to_string((uint32_t)ent));
			std::vector<ScriptComponent*> scripts = ent.GetComponent<ScriptComponentManager>().GetScripts();

			for (ScriptComponent* script : scripts)
			{
				MonoManager::CreateScriptEntity(&ent, script);
				//LOG_INFO("NEW INSTANCE: " + inst->GetScriptClass()->m_ClassName);
				//script->Script = inst;
			}

		}
		{
			auto& navs = GetAllEntitiesWith<NavMeshComponent>();
			NavMeshComponent* Nav = nullptr;
			for (auto& nm : navs)
			{
				//Nav = Entity{ nm, this }.GetComponent<NavMeshComponent>();
				Nav = &navs.get<NavMeshComponent>(nm);
				

				auto& view = GetAllEntitiesWith<AINavigatorComponent>();
				for (auto& comp : view)
				{
					Entity ent{ comp, this };
					AINavigatorComponent& currComp = ent.GetComponent<AINavigatorComponent>();
					currComp.RuntimeStart(ent.GetComponent<Rigidbody2DComponent>().RuntimeBody, Nav->mesh);
					currComp.agent->SetTarget(b2Vec2{ currComp.points[0]->x, currComp.points[0]->y});
				}
				break;
			}
		}

		auto& l = GetAllEntitiesWith<AudioListenerComponent>();
		if(l.empty() == false)
		{
			if (l.begin() != l.end())
			{
				Entity listener = { *l.begin() , this };
				listener.GetComponent<AudioListenerComponent>().UpdatePos(listener.GetComponent<TransformComponent>().Translation);
				listener.GetComponent<AudioListenerComponent>().ListenerUpdate(RuntimeSound->m_system);

			}
		}


		auto& sources = GetAllEntitiesWith<AudioSourceComponent>();
		if (sources.empty() == false)
		{
			for (auto e : sources)
			{
				Entity source = { e, this };


				source.GetComponent<AudioSourceComponent>().UpdatePos(source.GetComponent<TransformComponent>().Translation);
				source.GetComponent<AudioSourceComponent>().LoadSystem(RuntimeSound->m_system);
				if(source.GetComponent<AudioSourceComponent>().OnAwake)
					source.GetComponent<AudioSourceComponent>().Play();
				//source.GetComponent<AudioSourceComponent>().LoadSound(RuntimeSound->m_system);
				//source.GetComponent<AudioSourceComponent>().PlaySound(RuntimeSound->m_system);

			}
		}

		auto& anims = GetAllEntitiesWith<AnimationComponent>();
		if (anims.empty() == false)
		{
			for (auto e : anims)
			{
				Entity source = { e, this };

				source.GetComponent<AnimationComponent>().Play();

			}
		}
		RuntimeSound->m_system->update();
	}

	void Scene::OnRuntimeStop()
	{
		//once physics is added, the physcics world will be unloaded here
		//b2DestroyWorld(m_Physics2DWorld);
		PhysicsSystem::StopWorld();
		NetworkWorld->StopNetwork();
		delete NetworkWorld;

		RuntimeSound->m_channel->stop();
		MonoManager::ReloadMono();


		auto& group = GetAllEntitiesWith<AnimationComponent>();
		if (group.empty() == false)
		{
			for (auto e : group)
			{
				Entity source = { e, this };

				source.GetComponent<AnimationComponent>().Stop();

			}
		}
	}

	void Scene::SubmitToDestroyEntity(Entity entity)
	{
		bool isvalid = m_Registry.valid((entt::entity)entity);
		if (!isvalid)
		{
			LOG_WARN("Scene", "Trying to destroy entity {0}", (uint32_t)entity);
			return;
		}
		SubmitPostUpdateFunc([entity]() {entity.m_Scene->DestroyEntity(entity); });
	}


	void Scene::OnUpdateRuntime(Timestep ts)
	{
		PhysicsSystem::PhysicsUpdate((float)ts);

		//auto& scriptComponents = m_Registry.view<ScriptComponent>();	
		//for (auto& entity : scriptComponents)
		//{
		//	//const auto& script = scriptComponents.get<ScriptComponent>(entity);
		//	//LOG_INFO("[Update] : " + script.className);
		//	//MonoManager::CallScriptMethod(script.Script->GetManagedObject(), "Update");
		//
		//	Entity ent{ entity, this };
		//
		//	//auto& script = scriptComponents.get<ScriptComponent>(ent);
		//	//script.Script = MonoManager::CreateScriptEntity(std::filesystem::path(script.filePath).stem().string(), "Braveheart", uint32_t(ent));
		//	
		//	auto& script = ent.GetComponent<ScriptComponent>();
		//
		//	script.Script->InvokeOnUpdate((float)ts);
		//}

		auto& scriptManagers = m_Registry.view<ScriptComponentManager>();
		for (auto& comp : scriptManagers)
		{
			Entity ent{ comp, this };

			std::vector<ScriptComponent*> scripts = ent.GetComponent<ScriptComponentManager>().GetScripts();

			for (ScriptComponent* script : scripts)
			{
				//MonoManager::CallScriptMethod(script.Script->GetManagedObject(), "Update");
				if(script->Script != nullptr)
					script->Script->InvokeOnUpdate((float)ts);
				else 
				{
					LOG_ERROR("NO SCRIPT FOUND");
				}
			}

		}

		NetworkWorld->Update(this);


		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			//auto ents = GetAllEntitiesWith<RelationshipComponent, TransformComponent>();
			//for (auto e : ents)
			//{
			//	Entity otherEnt{ e, this };
			//	otherEnt.UpdateToParent();
			//}

			/*
			int subStepCount = 4;
			b2World_Step(m_Physics2DWorld, ts, subStepCount);

			b2ContactData contactData[10];


			auto physicsView = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : physicsView)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<Rigidbody2DComponent>();

				b2BodyId body = *(b2BodyId*)rb.RuntimeBody;
				const auto& pos = b2Body_GetPosition(body);
				transform.Translation.x = pos.x;
				transform.Translation.y = pos.y;

				b2Rot rotation = b2Body_GetRotation(body);
				transform.Rotation.z = b2Rot_GetAngle(rotation);

				b2Vec2 position = b2Body_GetPosition(body);
				rotation = b2Body_GetRotation(body);
				//printf("%4.2f %4.2f %4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));


				int bodyContactCapacity = b2Body_GetContactCapacity(body);

				int bodyContactCount = b2Body_GetContactData(body, contactData, 10);

				for (int i = 0; i < bodyContactCount; ++i)
				{
					b2ContactData* data = contactData + i;
					//printf("point count = %d\n", data->manifold.pointCount);
				}


			}


			b2ContactEvents contactEvents = b2World_GetContactEvents(m_Physics2DWorld);

			for (int i = 0; i < contactEvents.beginCount; ++i)
			{
				b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;

				b2Shape_GetBody(beginEvent->shapeIdA);
				b2Shape_GetBody(beginEvent->shapeIdB);

				uint32_t* A = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdA));
				uint32_t* B = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdB));

				Entity entA{ entt::entity((uint32_t)A), this };
				Entity entB{ entt::entity((uint32_t)B), this };

				std::cout << "Collider: " << std::endl;
				std::cout << entA.GetComponent<TagComponent>().Tag << std::endl;
				std::cout << "Other Collider: " << std::endl;
				std::cout << entB.GetComponent<TagComponent>().Tag << std::endl;

				//rb.ContactBegin(beginEvent->shapeIdA, beginEvent->shapeIdB);
			}

			*/
		}


		// Render Scene
		auto& view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : view)
		{
			auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.camera;
				cameraTransform = transform.GetTransform();
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);


			// Draw Sprites (Quads)
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), sprite.Colour, sprite.Thickness, sprite.Fade, (int)entity);

				}
			}

			Renderer2D::EndScene();
		}
		{
			auto& view = GetAllEntitiesWith<AINavigatorComponent>();
			for (auto& comp : view)
			{
				Entity ent{ comp, this };
				AINavigatorComponent& currComp = ent.GetComponent<AINavigatorComponent>();
				currComp.agent->Update(ts);
			}
		}


		RuntimeSound->m_system->update();

		//update audio
		auto& l = GetAllEntitiesWith<AudioListenerComponent>();
		if (!l.empty())
		{
			Entity listener = { *l.begin() , this };
			listener.GetComponent<AudioListenerComponent>().UpdatePos(listener.GetComponent<TransformComponent>().Translation);
			listener.GetComponent<AudioListenerComponent>().ListenerUpdate(RuntimeSound->m_system);

		}

		auto& sources = m_Registry.view<AudioSourceComponent>();
		if (sources.empty() == false)
		{
			for (auto e : sources)
			{
				Entity source = { e, this };

				source.GetComponent<AudioSourceComponent>().UpdatePos(source.GetComponent<TransformComponent>().Translation);
				//source.GetComponent<AudioSourceComponent>().PlayOneShot();

			}
		}


		auto& anims = GetAllEntitiesWith<AnimationComponent>();
		if (anims.empty() == false)
		{
			for (auto e : anims)
			{
				Entity source = { e, this };
				m_Registry.get<AnimationComponent>(e);

				source.GetComponent<AnimationComponent>().Update(ts, (uint32_t)source, this);

			}
		}

		RuntimeSound->m_system->update();



		//callig post update methods
		for (auto&& fn : m_PostUpdateQueue)
		{
			fn();
		}
		m_PostUpdateQueue.clear();

	}

	void Scene::CopyRelationships(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto srcEntities = srcRegistry.view<RelationshipComponent>();
		for (auto srcEntity : srcEntities)
		{
			UUID uuid = srcRegistry.get<IDComponent>(srcEntity).ID;
			entt::entity destEntity = enttMap.at(uuid);

			// If the entity has a parent, copy the parent-child relationship
			if (srcRegistry.any_of<RelationshipComponent>(srcEntity))
			{
				const auto& relComp = srcRegistry.get<RelationshipComponent>(srcEntity);
				if (relComp.parent != entt::null)
				{
					// Convert the parent uint32_t ID to an entity handle
					entt::entity parentEntity = static_cast<entt::entity>(relComp.parent);

					// Ensure the parent entity exists in the new registry
					if (enttMap.find(srcRegistry.get<IDComponent>(parentEntity).ID) != enttMap.end())
					{
						parentEntity = enttMap.at(srcRegistry.get<IDComponent>(parentEntity).ID);
					}

					// Set the parent in the destination entity
					dstRegistry.emplace_or_replace<RelationshipComponent>(destEntity, RelationshipComponent{ static_cast<uint32_t>(parentEntity) });
				}
			}
		}
	}


	/*
	void Scene::OnUpdate(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		{
			// Render Scene
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			// Draw Sprites (Quads)
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			Renderer2D::EndScene();
		}
	}
	*/

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-fixed aspect ratio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.FixedAspectRatio == false)
			{
				// Resize the camera
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}

}