#pragma once
#include <iostream>
#include <Enet/enet.h>

namespace Engine
{
    struct Vector3;

    class NetworkHost 
    {
    public:
        NetworkHost();
        ~NetworkHost();

        void StartServer(std::string IPAddress = "127.0.0.1", enet_uint16 port = 5046);
        void ServerUpdate();
        void StopServer();
        void HandleClientData();
        void SendToClients(const std::string& message);
        void SendToClient(ENetPeer* peer, const std::string& message);

    public:
        ENetHost* m_server;  // Server host (for accepting connections)
        ENetEvent m_event;   // Store events like client connections, disconnections, etc.
    
        static int GlobalID;

        std::unordered_map<int, Vector3> clients;

        std::string latestMessage;

    
    };
}