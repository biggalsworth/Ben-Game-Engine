#pragma once
#include <Box2D/box2d.h>
#include "Raycast.h"
namespace Engine
{
	class Scene;
	class Entity;

	//class SensorListenert : public b2SensorBeginTouchEvent {
	//public:
	//	void BeginContact(b2Contact* contact) override {
	//		b2Fixture* fixtureA = contact->GetFixtureA();
	//		b2Fixture* fixtureB = contact->GetFixtureB();
	//
	//		if (fixtureA->IsSensor()) {
	//			// Handle sensor event for fixtureA
	//		}
	//		if (fixtureB->IsSensor()) {
	//			// Handle sensor event for fixtureB
	//		}
	//	}
	//
	//	void EndContact(b2Contact* contact) override {
	//		// Handle when sensor overlap ends
	//	}
	//};
	
	static struct RayCastHit
	{
		uint32_t shapeId;
		float point[2];
		float normal[2];
		float fraction;
		int nodeVisits;
		int leafVisits;
		int count;
	};

	static struct CastData
	{
		b2Vec2 points[3];
		b2Vec2 normals[3];
		float fractions[3];
		int count;
		uint32_t entity[3];
	};

	//static float RayCallBack(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
	//
	//	RayCastHit* myContext = static_cast<RayCastHit*>(context);
	//	myContext->shapeId = shapeId;
	//	myContext->point = point;
	//	myContext->normal = normal;
	//	myContext->fraction = fraction;
	//	return fraction;
	//

	static class PhysicsSystem
	{

	public:
		static struct ShapeData
		{
			ShapeData() = default;
			uint32_t entity;
			bool ignore = false;
		};


		PhysicsSystem() {};
		~PhysicsSystem() {};

		static void SetWorld(Scene* mainScene, entt::registry* registry);

		inline static void StopWorld()
		{
			b2DestroyWorld(m_PhysicsWorld);	
		}

		static void StartWorld();
		

		static void PhysicsUpdate(float ts);

		static void BuildRigidbody(Entity entity);
		static void DestroyBody(Entity entity);

		static RayCastHit RayCast(b2Vec2 origin, b2Vec2 end, RayCast2D::CastType type = RayCast2D::CastType::RayCastClosest);

		static std::vector<uint32_t> CircleCast(b2Vec2 origin, float rotation, float radius, RayCast2D::CastType type = RayCast2D::CastType::RayCastMultiple);

	public:
		inline static b2WorldId m_PhysicsWorld;
		inline static Scene* m_ActiveScene;
		inline static entt::registry* m_Registry;
		//inline static b2WorldDef m_PhysicsWorld;

	};
}

