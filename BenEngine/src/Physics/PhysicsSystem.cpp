#include "Engine_PCH.h"
#include "PhysicsSystem.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Engine
{
	
	void PhysicsSystem::SetWorld(Scene* mainScene, entt::registry* registry)
	{
		m_ActiveScene = mainScene;
		m_Registry = registry;

		b2Vec2 gravity({ 0.0f, -9.8f }); // Define gravity
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, -9.8f };

		m_PhysicsWorld = b2WorldId(b2CreateWorld(&worldDef));
	}
	void PhysicsSystem::StartWorld()
	{
		auto view = m_Registry->view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, m_ActiveScene };

			BuildRigidbody(entity);

		}
	}

	void PhysicsSystem::PhysicsUpdate(float ts)
	{
		int subStepCount = 4;
		b2World_Step(m_PhysicsWorld, ts, subStepCount);

		b2SensorArray triggerData[10];
		//RayCastHit hitout;
		//RayCast2D({ 0,0 }, { 0, -1 }, &hitout);


		auto physicsView = m_Registry->view<Rigidbody2DComponent>();
		for (auto e : physicsView)
		{

			Entity entity = { e, m_ActiveScene };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb = entity.GetComponent<Rigidbody2DComponent>();

			if (rb.RuntimeBody == nullptr)
			{
				BuildRigidbody(entity);
				continue;
			}
			b2BodyId* body = (b2BodyId*)rb.RuntimeBody;

			if (rb.Type != Rigidbody2DComponent::BodyType::Static)
			{
				const auto& pos = b2Body_GetPosition(*body);
				transform.Translation.x = pos.x;
				transform.Translation.y = pos.y;

				b2Rot rotation = b2Body_GetRotation(*body);
				transform.Rotation.z = b2Rot_GetAngle(rotation);

				b2Vec2 position = b2Body_GetPosition(*body);
				rotation = b2Body_GetRotation(*body);
				//update bodies
				if (b2Body_GetMass(*body) != rb.Mass || b2Body_GetGravityScale(*body) != rb.GravityScale)
				{
					b2Body_SetGravityScale(*body, rb.GravityScale);
					b2Body_SetMassData(*body, b2MassData{ rb.Mass });
					b2Body_SetAwake(*body, true);
				}
			}
			if (b2Shape_IsSensor(*rb.RuntimeShape) == false)
			{
				//checking for collisions
				b2ContactData contactData[10];
				int bodyContactCapacity = b2Body_GetContactCapacity(*body);
				int bodyContactCount = b2Body_GetContactData(*body, contactData, 10);
				if (bodyContactCount <= 0)
					continue;

				//looping through all collisions
				for (int i = 0; i < bodyContactCount; ++i)
				{
					b2ContactData* data = contactData + i;
					printf("point count = %d\n", data->manifold.pointCount);
					b2Shape_GetBody(data->shapeIdA);
					b2Shape_GetBody(data->shapeIdB);

					//recieve the entity data
					//uint32_t* A = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdA));
					ShapeData* A = reinterpret_cast<ShapeData*>((ShapeData*)b2Shape_GetUserData(data->shapeIdA));
					ShapeData* B = reinterpret_cast<ShapeData*>((ShapeData*)b2Shape_GetUserData(data->shapeIdB));
					//uint32_t* B = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdB));

					Entity entA{ entt::entity((uint32_t)A->entity), m_ActiveScene };
					Entity entB{ entt::entity((uint32_t)B->entity), m_ActiveScene };


					LOG_WARN("Collider: " + entA.GetComponent<TagComponent>().Tag);
					LOG_WARN("Other Collider: " + entB.GetComponent<TagComponent>().Tag);

					//call the collision function using mono on both entities

					Entity ent = Entity{ (entt::entity)A->entity, m_ActiveScene };
					for (ScriptComponent* script : ent.GetComponent<ScriptComponentManager>().GetScripts())
					{
						MonoMethod* onCollisionEnterMethod = mono_class_get_method_from_name(script->Script->GetScriptClass()->m_MonoClass, "OnCollisionEnter", 1);
						MonoObject* scriptInstance = script->Script->GetManagedObject(); // Get the correct script instance

						if (onCollisionEnterMethod)
						{
							uint32_t entityID = (uint32_t)B->entity;
							void* param = &entityID;
							script->Script->GetScriptClass()->InvokeMethod(scriptInstance, onCollisionEnterMethod, &param);
						}

					}
					ent = Entity{ (entt::entity)B->entity, m_ActiveScene };
					for (ScriptComponent* script : ent.GetComponent<ScriptComponentManager>().GetScripts())
					{
						MonoMethod* onCollisionEnterMethod = mono_class_get_method_from_name(script->Script->GetScriptClass()->m_MonoClass, "OnCollisionEnter", 1);
						MonoObject* scriptInstance = script->Script->GetManagedObject(); // Get the correct script instance

						if (onCollisionEnterMethod)
						{
							uint32_t entityID = (uint32_t)A->entity;
							void* param = &entityID;
							script->Script->GetScriptClass()->InvokeMethod(scriptInstance, onCollisionEnterMethod, &param);
						}

					}
				}
			}
			else
			{
				//recive trigger enters
				b2Shape_GetSensorCapacity(*rb.RuntimeShape);
				b2ShapeId hits[10];
				//int overlaps = b2Shape_GetSensorOverlaps(*rb.RuntimeShape, hits, b2Shape_GetSensorCapacity(*rb.RuntimeShape));
				int overlaps = b2Shape_GetSensorOverlaps(*rb.RuntimeShape, hits, 10);

				for (int i = 0; i < overlaps; ++i)
				{
					ShapeData* hit = reinterpret_cast<ShapeData*>((ShapeData*)b2Shape_GetUserData(hits[i]));
					Entity overlapped{ entt::entity((uint32_t)hit->entity), m_ActiveScene };
					//LOG_WARN("overlapped with : " + overlapped.GetComponent<TagComponent>().Tag);

					for (ScriptComponent* script : overlapped.GetComponent<ScriptComponentManager>().GetScripts())
					{
						MonoMethod* onCollisionEnterMethod = mono_class_get_method_from_name(script->Script->GetScriptClass()->m_MonoClass, "OnTriggerEnter", 1);
						MonoObject* scriptInstance = script->Script->GetManagedObject(); // Get the correct script instance

						if (onCollisionEnterMethod)
						{
							uint32_t entityID = (uint32_t)entity;
							void* param = &entityID;
							script->Script->GetScriptClass()->InvokeMethod(scriptInstance, onCollisionEnterMethod, &param);
						}

					}

				}
			}

			


		}


		//b2ContactEvents contactEvents = b2World_GetContactEvents(m_PhysicsWorld);
		//
		//for (int i = 0; i < contactEvents.beginCount; ++i)
		//{
		//	b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
		//
		//	b2Shape_GetBody(beginEvent->shapeIdA);
		//	b2Shape_GetBody(beginEvent->shapeIdB);
		//
		//	//uint32_t* A = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdA));
		//	ShapeData* A = reinterpret_cast<ShapeData*>((ShapeData*)b2Shape_GetUserData(beginEvent->shapeIdA));
		//	ShapeData* B = reinterpret_cast<ShapeData*>((ShapeData*)b2Shape_GetUserData(beginEvent->shapeIdB));
		//	//uint32_t* B = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(beginEvent->shapeIdB));
		//
		//	Entity entA{ entt::entity((uint32_t)A->entity), m_ActiveScene };
		//	Entity entB{ entt::entity((uint32_t)B->entity), m_ActiveScene };
		//
		//
		//	LOG_WARN("Collider: " + entA.GetComponent<TagComponent>().Tag);
		//	LOG_WARN("Other Collider: " + entB.GetComponent<TagComponent>().Tag);
		//
		//
		//	Entity ent = Entity{ (entt::entity)A->entity, m_ActiveScene };
		//	for (ScriptComponent* script : ent.GetComponent<ScriptComponentManager>().GetScripts())
		//	{
		//		MonoMethod* onCollisionEnterMethod = mono_class_get_method_from_name(script->Script->GetScriptClass()->m_MonoClass, "OnCollisionEnter", 1);
		//		MonoObject* scriptInstance = script->Script->GetManagedObject(); // Get the correct script instance
		//		
		//		if (onCollisionEnterMethod)
		//		{
		//			uint32_t entityID = (uint32_t)A->entity;
		//			void* param = &entityID;
		//			script->Script->GetScriptClass()->InvokeMethod(scriptInstance, onCollisionEnterMethod, &param);
		//		}
		//
		//	}
		//
		//	ent = Entity{ (entt::entity)B->entity, m_ActiveScene };
		//	for (ScriptComponent* script : ent.GetComponent<ScriptComponentManager>().GetScripts())
		//	{
		//		MonoMethod* onCollisionEnterMethod = mono_class_get_method_from_name(script->Script->GetScriptClass()->m_MonoClass, "OnCollisionEnter", 1);
		//		MonoObject* scriptInstance = script->Script->GetManagedObject(); // Get the correct script instance
		//		
		//		if (onCollisionEnterMethod)
		//		{
		//			uint32_t entityID = (uint32_t)B->entity;
		//			void* param = &entityID;
		//			script->Script->GetScriptClass()->InvokeMethod(scriptInstance, onCollisionEnterMethod, &param);
		//		}
		//
		//	}
		//
		//	//rb.ContactBegin(beginEvent->shapeIdA, beginEvent->shapeIdB);
		//}


		b2World_Step(m_PhysicsWorld, ts, subStepCount);

	}
	void PhysicsSystem::BuildRigidbody(Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb = entity.GetComponent<Rigidbody2DComponent>();

		//b2BodyDef bodyDef;
		//bodyDef.type = (b2BodyType)rb.Type;

		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = (b2BodyType)rb.Type;
		bodyDef.position = b2Vec2({ (transform.WorldPos.x + transform.Translation.x), (transform.WorldPos.y + transform.Translation.y) });
		bodyDef.rotation = b2MakeRot(transform.Rotation.z);
		bodyDef.fixedRotation = rb.fixedRotation;
		bodyDef.userData = reinterpret_cast<void*>((intptr_t)((uint32_t)entity));

		bodyDef.gravityScale = rb.GravityScale;
		bodyDef.linearDamping = rb.LinearDamping;
		bodyDef.angularDamping = rb.AngularDamping;

		b2BodyId* body = new b2BodyId(b2CreateBody(m_PhysicsWorld, &bodyDef));
		b2Body_SetMassData(*body, b2MassData{ rb.Mass });
		b2Body_SetGravityScale(*body, rb.GravityScale);

		rb.RuntimeBody = body;


		//b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);

		if (entity.HasComponent<BoxCollider2DComponent>())
			rb.RuntimeShape = new b2ShapeId(entity.GetComponent<BoxCollider2DComponent>().Build(body, transform.Scale + transform.WorldScale, (uint32_t)entity));

		if (entity.HasComponent<CircleCollider2DComponent>())
			rb.RuntimeShape = new b2ShapeId(entity.GetComponent<CircleCollider2DComponent>().Build(body, transform.Scale + transform.WorldScale, (uint32_t)entity));

	}

	void PhysicsSystem::DestroyBody(Entity entity)
	{
		b2DestroyBody(*entity.GetComponent<Rigidbody2DComponent>().RuntimeBody);
	}

	RayCastHit PhysicsSystem::RayCast(b2Vec2 origin, b2Vec2 end, RayCast2D::CastType type)
	{
		RayCast2D::RayCastContext hitOut;
		//LOG_WARN("END: " + std::to_string(end.y));
		hitOut = RayCast2D::RayCast(m_PhysicsWorld, origin, end, type);

		//Renderer2D::DrawLine(glm::vec3(origin.x, origin.y, 0), glm::vec3(end.x, end.y, 0), glm::vec4(1.0f, 0.1f, 0.1f, 1.0f),4);

		RayCastHit hit;
		hit.shapeId = hitOut.entity[0];
		hit.fraction = hitOut.fractions[0];
		hit.normal[0] = hitOut.normals[0].x;
		hit.normal[1] = hitOut.normals[0].y;
		hit.count = hitOut.count;

		//LOG_WARN("HIT " + std::to_string(hit.count	));

		return hit;
	}

	std::vector<uint32_t> PhysicsSystem::CircleCast(b2Vec2 origin, float rotation, float radius, RayCast2D::CastType type)
	{
		return RayCast2D::CircleCast(m_PhysicsWorld, origin, rotation, radius, type);
	}
}