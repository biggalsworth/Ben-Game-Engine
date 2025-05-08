#include "Engine_PCH.h"
#include "NetworkHost.h"
#include "NetworkManager.h"

namespace Engine
{
    int NetworkHost::GlobalID = 1000;

    NetworkHost::NetworkHost()
        : m_server(nullptr) {
        // Initialize ENet
        if (enet_initialize() != 0) {
            LOG_ERROR("Failed to initialize ENet.\n");
            exit(EXIT_FAILURE);
        }
        LOG_INFO("ENet initialized.\n");
        m_server = enet_host_create(NULL, 32, 2, 0, 0);

    }

    NetworkHost::~NetworkHost() {
        if (m_server) {
            enet_host_destroy(m_server);
            //m_server = nullptr;
        }
        enet_deinitialize();
        std::cout << "ENet deinitialized.\n";
    }

    void NetworkHost::StartServer(std::string IPAddress, enet_uint16 port)
    {
        ENetAddress address;
        enet_address_set_host(&address, IPAddress.c_str());  // Bind to all interfaces
        address.port = port;

        // Create a server host (listen for up to 32 clients, max 2 channels per client)
        m_server = enet_host_create(&address, 32, 2, 0, 0);
        if (!m_server) {
            LOG_INFO("Failed to create server host.\n");
            return;
        }

        LOG_INFO("Server started and listening");
        LOG_INFO("Server IP: " + IPAddress);
        LOG_INFO("Server port: " + std::to_string(port));


        // Handle recieving messages in a thread as to not block the system
        std::thread serverThread([this]() { HandleClientData(); });
        serverThread.detach();
    }

    void NetworkHost::ServerUpdate()
    {
        //HandleClientData();
    }

    void NetworkHost::StopServer()
    {
        if (!m_server) {
            std::cerr << "Error: m_server is nullptr!" << std::endl;
            return;
        }

        // Disconnect all connected clients
        for (ENetPeer* peer = m_server->peers; peer < &m_server->peers[m_server->peerCount]; ++peer) {
            if (peer->state != ENET_PEER_STATE_DISCONNECTED) {
                std::cout << "Disconnecting client " << peer->address.host << ":" << peer->address.port << "\n";
                enet_peer_disconnect(peer, 0);  // Gracefully disconnect the peer with a reason of 0 (optional)
            }
        }

        // Wait 5000 ms for any packets that need to be sent before shutting down
        ENetEvent event;
        while (enet_host_service(m_server, &event, 5000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected\n";
                break;
            default:
                break;
            }
        }

        std::cout << "Server stopped successfully.\n";
    }

    void NetworkHost::HandleClientData() 
    {
        ClientData* clientData;

        ENetEvent event;
        while (true) {
            while (enet_host_service(m_server, &event, 1000) > 0) 
            {
                m_event = event;
                switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    LOG_INFO("Client connected: " + std::to_string(m_event.peer->address.host) + ":" + std::to_string(m_event.peer->address.port));
                    m_event.peer->data = (void*)GlobalID;  // Store some data with the peer
                    SendToClient(m_event.peer, "ID\n" + std::to_string(GlobalID));
                    SendToClients("NEW PLAYER\n" + std::to_string(GlobalID));
                    GlobalID++;
                    break;

                case ENET_EVENT_TYPE_RECEIVE: {
                    // Handle packet reception
                    std::string message(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
                    LOG_INFO("Message Recieved: " + message);
                    latestMessage = message;
                    // Destroy the received packet after processing
                    enet_packet_destroy(m_event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected.\n";
                    break;

                default:
                    break;
                }
            }
        }
    }

    void NetworkHost::SendToClients(const std::string& message) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.size() + 1, ENET_PACKET_FLAG_RELIABLE);

        for (ENetPeer* peer = m_server->peers; peer < &m_server->peers[m_server->peerCount]; ++peer) {
            if (peer->state != ENET_PEER_STATE_DISCONNECTED) {
                enet_peer_send(peer, 0, packet);
            }
        }
        enet_host_flush(m_server);
        std::cout << "Sent message to clients: " << message << "\n";
    }

    void NetworkHost::SendToClient(ENetPeer* peer, const std::string& message) 
    {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        if (!packet) {
            std::cerr << "Failed to create ENet packet.\n";
            return;
        } 
        if (enet_peer_send(peer, 0, packet) != 0) {
            LOG_ERROR("Failed to send packet to client.\n");
        }
    }

}