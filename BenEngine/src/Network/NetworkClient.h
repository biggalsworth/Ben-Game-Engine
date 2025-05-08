#pragma once
#include <iostream>
#include <Enet/enet.h>

namespace Engine
{
	class Scene;


	class NetworkClient
	{
	public:
		NetworkClient();
		~NetworkClient() = default;

	public:
		void Connect(Scene* context, std::string hostAddress = "127.0.0.1", enet_uint16 _port = 5046);
		void Disconnect();
		void MessageServer(std::string Data);
		void SendData(std::string Data);

		void ReadData();

	public:
		int GlobalID = 0;
		std::string latestIncoming;


	private:
		int clientIndex = -1;
		ENetAddress m_address;
		ENetHost*	m_client;
		ENetPeer*	m_peer;
		ENetEvent	m_enetEvent;
		Scene* m_context;
	};
}