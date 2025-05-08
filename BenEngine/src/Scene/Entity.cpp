#include "Engine_PCH.h"
#include "Entity.h"

namespace Engine
{
	Entity::Entity(entt::entity handle_, Scene* scene_) : 
		m_EntityHandle(handle_), m_Scene(scene_)
	{

	}

	UUID Entity::GetSceneUUID() const
	{
		return m_Scene->m_SceneID;
	}


	void Entity::UpdateToParent()
	{
		try
		{
			// Check if the entity has a parent and if the parent is valid
			if (HasComponent<RelationshipComponent>() && (uint32_t)GetComponent<RelationshipComponent>().parent != entt::null)
			{
				Entity* parent = &this->GetParent();
				if ((uint32_t)parent != entt::null && parent->HasComponent<RelationshipComponent>())
				{
					const auto& pr = parent->GetComponent<RelationshipComponent>();
					const auto& pt = parent->GetComponent<TransformComponent>();

					////iterative loop for all hierarichal parents
					//glm::mat4 transform = pt.GetLocalTransform();
					//glm::vec3 translation, rotation, scale;
					//Math::DecomposeTransform(transform, translation, rotation, scale);
					//
					//// Get the parent's global transformation matrix
					//while ((uint32_t)parent->GetParent() != (uint32_t)entt::null)
					//{
					//	transform = parent->GetComponent<TransformComponent>().GetTransform() + transform;
					//	if ((uint32_t)parent->GetParent() == (uint32_t)entt::null || !parent->HasComponent<RelationshipComponent>())
					//	{
					//		//we found the master parent, leave the loop
					//		break;
					//	}
					//	*parent = parent->GetParent();
					//
					//	std::cout << "NULL: " << std::endl;
					//
					//	std::cout << std::to_string((uint32_t)entt::null).c_str() << std::endl;
					//
					//	if((uint32_t)parent->GetParent() == (uint32_t)entt::null)
					//	{
					//		std::cout << "DOESNT EXIST " << std::endl;
					//	}
					//
					//	std::cout << "This id: " << std::to_string((uint32_t)parent).c_str() << std::endl;
					//	std::cout << "This parent: " << std::to_string((uint32_t)parent->GetParent()).c_str() << std::endl;
					//	std::cout << std::to_string((uint32_t)m_Scene).c_str() << std::endl;
					//}
					//
					////transform = this->GetParent().GetComponent<TransformComponent>().GetTransform() + 
					//
					//Math::DecomposeTransform(transform, translation, rotation, scale);

					//GetComponent<TransformComponent>().WorldPos = translation;
					//GetComponent<TransformComponent>().WorldRot = rotation;
					//GetComponent<TransformComponent>().WorldScale = pt.Scale + pt.WorldScale - glm::vec3(1.0);

					GetComponent<TransformComponent>().WorldPos = pt.WorldPos + pt.Translation;
					GetComponent<TransformComponent>().WorldRot = pt.Rotation + pt.WorldRot;
					GetComponent<TransformComponent>().WorldScale = pt.Scale + pt.WorldScale - glm::vec3(1.0);

					throw 505;
				}
			}
		}
		catch (...)
		{
			//couldn't find parent
		}
	}
}