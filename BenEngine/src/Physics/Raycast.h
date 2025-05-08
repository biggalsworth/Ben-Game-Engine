#pragma once
#include "box2d/base.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath> // For sinf and cosf
#include <box2d/box2d.h> // Ensure Box2D is included
#include "gl/GL.h"
#include <iostream>

#include "entt.hpp"

namespace Engine
{

	static class RayCast2D
	{
	public:

		struct RayCastContext
		{
			b2Vec2 points[3];
			b2Vec2 normals[3];
			float fractions[3];
			int count;
			uint32_t entity[3];
		};

		struct OverlapResult 
		{
			uint32_t shapeId;
		};

		struct OverlapContacts
		{
			std::vector<uint32_t> shapeId;
		};


		enum CastType
		{
			RayCastAny = 0,
			RayCastClosest = 1,
			RayCastMultiple = 2,
			RayCastSorted = 3
		};

		enum Mode
		{
			e_any = 0,
			e_closest = 1,
			e_multiple = 2,
			e_sorted = 3
		};

	private:
		// This callback finds the closest hit. This is the most common callback used in games.
		static float RayCastClosestCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);

		// This callback finds any hit. For this type of query we are usually just checking for obstruction,
		// so the hit data is not relevant.
		// NOTE: shape hits are not ordered, so this may not return the closest hit
		static float RayCastAnyCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);

		// This ray cast collects multiple hits along the ray.
		// The shapes are not necessary reported in order, so we might not capture
		// the closest shape.
		// NOTE: shape hits are not ordered, so this may return hits in any order. This means that
		// if you limit the number of results, you may discard the closest hit. You can see this
		// behavior in the sample.
		static float RayCastMultipleCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);
		
		// This ray cast collects multiple hits along the ray and sorts them.
		static float RayCastSortedCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);

		// Callback function for overlap results
		//static bool CircleOverlap(b2ShapeId shapeId, void* context);
		static bool CircleOverlap(b2ShapeId shapeId, void* context);



		const static b2CastResultFcn* fcns[];

		RayCastContext context = { };

		static std::vector<uint32_t> overlapResults;

	public:
		static RayCastContext RayCast(b2WorldId world, b2Vec2 origin, b2Vec2 end, CastType m_castType)
		{
			b2Vec2 translation = b2Sub(end, origin);

			float magnitude = b2Length(translation);
			if (magnitude > 0.0f)
			{
				b2Normalize(translation);// Use Box2D's Normalize method to get the direction
			}


			b2CastResultFcn* modeFcn = fcns[m_castType];
			RayCastContext context = { -1 };


			b2World_CastRay(world, origin, translation, b2DefaultQueryFilter(), modeFcn, &context);


			return context;
		};



		static std::vector<uint32_t> CircleCast(b2WorldId world, b2Vec2 origin, float rotation, float radius, CastType m_castType)
		{

			overlapResults.clear(); //clear past hits

			//define the circle
			b2Circle circle = { {0,0}, radius};

			b2Transform transform = { origin, b2MakeRot(rotation) };

			// Perform overlap test
			b2QueryFilter filter; // Define filter criteria if needed
			b2World_OverlapCircle(world, &circle, transform, b2DefaultQueryFilter(), *CircleOverlap, nullptr);

			return overlapResults;

		}
	};
}