#pragma once
#include "Scene.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void Serialise(const std::string& filePath);
		bool Deserialise(const std::string& filePath);
		void EntityDeserialise(YAML::Node data);
		void EntitySerialise(const std::string& filePath, entt::entity entityID);
		void EntityLoad(const std::string& filePath);

	private:
		Ref<Scene> m_Scene;
	};
}