#include "Engine_PCH.h"
#include "NetworkClient.h"
#include "NetworkManager.h"

namespace Engine
{

	NetworkClient::NetworkClient()
	{
		if (enet_initialize() != 0) {
			std::cerr << "Failed to initialize ENet.\n";
			exit(EXIT_FAILURE);
		}
		std::cout << "ENet initialized.\n";
		m_client = nullptr;
		m_peer = nullptr;
	}

	void NetworkClient::Connect(Scene* context, std::string hostName, enet_uint16 _port)
	{
		m_client = enet_host_create(nullptr, 1, 2, 0, 0);
		if (m_client == nullptr) {
			LOG_INFO("Client failed to initialize!\n");
			return;
		}

		m_address.host = ENET_HOST_ANY;
		m_address.port = _port;

		if (enet_address_set_host(&m_address, hostName.c_str()) != 0) {
			LOG_INFO("Failed to resolve host: %s\n", hostName.c_str());
			return;
		}

		m_peer = enet_host_connect(m_client, &m_address, 2, 0);
		if (m_peer == nullptr) {
			LOG_INFO("No available peers for initializing an ENet connection.\n");
			return;
		}

		LOG_INFO("Attempting to connect to " + hostName + " : " + std::to_string(_port));

		ENetEvent event;
		if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			LOG_INFO("Successfully connected to %s:%u\n", hostName.c_str(), _port);
			MessageServer("HELLO FROM CLIENT");
		}
		else {
			LOG_INFO("Failed to connect to " + hostName + " : " + std::to_string(_port));
			enet_peer_reset(m_peer);
		}
	}

	void NetworkClient::Disconnect()
	{
		enet_peer_reset(m_peer);
	}

	void NetworkClient::MessageServer(std::string Data)
	{
		if (!m_peer) {
			LOG_INFO("Cannot send data. Not connected to a server.\n");
			return;
		}

		ENetPacket* dataPacket = enet_packet_create(Data.c_str(), Data.size() + 1, ENET_PACKET_FLAG_RELIABLE); // +1 to include the null terminator
		if (!dataPacket) {
			LOG_INFO("Failed to create ENet packet.\n");
			return;
		}

		// Send the packet to the server
		if (enet_peer_send(m_peer, 0, dataPacket) != 0) {
			LOG_INFO("Failed to send packet to server.\n");
		}
		else {
			LOG_INFO("Packet sent successfully.\n");
		}

		enet_host_flush(m_client); // Ensure the packet is sent immediately
	}

	void NetworkClient::SendData(std::string Data)
	{
		if (!m_peer) {
			LOG_INFO("Cannot send data. Not connected to a server.\n");
			return;
		}

		// Create and populate the packet
		ClientPacket clientPacket;
		clientPacket.clientIndex = clientIndex;
		clientPacket.position = { 0.0f, 0.0f, 0.0f }; // Set default position

		ENetPacket* dataPacket = enet_packet_create(&clientPacket, sizeof(ClientPacket), ENET_PACKET_FLAG_RELIABLE);
		if (!dataPacket) {
			LOG_INFO("Failed to create ENet packet.\n");
			return;
		}

		// Send the packet to the server
		if (enet_peer_send(m_peer, 0, dataPacket) != 0) {
			LOG_INFO("Failed to send packet to server.\n");
		}
		else {
			LOG_INFO("Packet sent successfully.\n");
		}

		enet_host_flush(m_client); // Ensure the packet is sent immediately
	}


	void NetworkClient::ReadData()
	{
		ClientData* clientData = new ClientData;
		//Define type of packet
		int* packetType = new int;
		*packetType = -1;
		
		while (enet_host_service(m_client, &m_enetEvent, 0) > 0)
		{
			switch (m_enetEvent.type) 
			{
			case ENET_EVENT_TYPE_CONNECT:
				LOG_INFO("Successfully connected to the server!\n");
				break;

				/* Again, we're reacting based on the detected event type. In this case, it's
				ENET_EVENT_TYPE_RECEIVE, which means our client has recieved a packet from a
				peer (in this case, the server). After quickly outputting some debug text to
				console to confirm packet receipt, what happens next is the key part.
		
				Our packet has some intrinsic variables - its data (what it's storing) and its
				dataLength (how much data it's storing). In this case, since we KNOW the packet
				is a Vector2, we can use the memcpy function fairly easily. This is a basic C++
				function which copies a given amount of data from one location to another. In
				this case, it copies TO newPosition FROM data, and it copies an amount dataLength.
		
				Given what we know about pointer arithmetic, it should be obvious to us that we
				can make these packets more sophisticated. We can make huge packets which hold
				many different kinds of data. We simply include an enumerator at the beginning of
				each data segment, saying what data type it is, and either copy that much data over
				into a variable of that type, or include as the next element of a packet the amount
				of data this variable type needs to copy. This is particularly useful when it comes
				to Part 2 of the coursework, where 'level data' is likely very different to the
				'physics data' you'll have been transmitting for Part 1. */
		
			case ENET_EVENT_TYPE_RECEIVE:

				std::string receivedMessage(reinterpret_cast<char*>(m_enetEvent.packet->data), m_enetEvent.packet->dataLength);
				LOG_INFO("Message Received From Server: " + receivedMessage);

				std::vector<std::string> message = SplitData(receivedMessage);
				latestIncoming = receivedMessage;

				if (message[0] == "ID")
				{
					GlobalID = std::stoi(message[1]);
				}
				enet_packet_destroy(m_enetEvent.packet);

				//memcpy(packetType, m_enetEvent.packet->data, sizeof(int));
				//
				////we have recieved a packet - assign the clientIndex
				//if (*packetType == 0)
				//{
				//	memcpy(clientData, m_enetEvent.packet->data, sizeof(ClientData));
				//	clientIndex = clientData->clientIndex;
				//}
				//else if (*packetType == 1) //we recieved some kind of data
				//{
				//	
				//}
		
				break;
			}
		}
	}

}