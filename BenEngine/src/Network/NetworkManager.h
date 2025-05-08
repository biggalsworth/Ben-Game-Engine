#pragma once
#include "Engine.h"

namespace Engine
{
	class Scene;
	
	struct Vector2 {
		float x;
		float y;
	};
	struct Vector3 {
		float x;
		float y;
		float z;
	};


	struct ClientData {
		int packetType = 0;
		int clientIndex;
	};

	struct ClientPacket {
		int clientIndex;
		Vector3 position;
	};

	struct ClientMessagePacket {
		int clientIndex;
		std::string message;
	};
	
	//split messages by line
	static std::vector<std::string> SplitData(const std::string& input) {
		std::vector<std::string> lines;
		std::stringstream ss(input);
		std::string line;

		while (std::getline(ss, line, '\n')) {
			lines.push_back(line);
		}

		return lines;
	}

	class NetworkManager
	{
	public:
		NetworkManager(Scene* context) : m_Context(context) {};
		~NetworkManager() = default;

		void SetupNetwork();
		void Update(Scene* Context);
		void StopNetwork();



	public:
		Scene* m_Context;
	};
}