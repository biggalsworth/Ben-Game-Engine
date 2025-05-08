#include "Engine_PCH.h"
#include "NetworkManager.h"

namespace Engine
{
	void NetworkManager::SetupNetwork()
	{
		auto& Hosts = m_Context->GetAllEntitiesWith<NetworkHostComponent>();
		if (Hosts.empty() == false)
		{
			for (auto& Host : Hosts)
			{
				Entity CurrEnt{ Host, m_Context };
				NetworkHostComponent* comp = &CurrEnt.GetComponent<NetworkHostComponent>();
				//if (comp->server == nullptr)
				comp->server = new NetworkHost();
				comp->server->StartServer(comp->IPAddress, comp->port);
			}
		}

		auto& Clients = m_Context->GetAllEntitiesWith<NetworkClientComponent>();
		if (Clients.empty() == false)
		{
			for (auto& Client : Clients)
			{
				Entity CurrEnt{ Client, m_Context };
				NetworkClientComponent* comp = &CurrEnt.GetComponent<NetworkClientComponent>();

				if (comp->connection == nullptr)
					comp->connection = new NetworkClient();
				comp->connection->Connect(m_Context, comp->IPAddress, comp->port);
				comp->globalID = comp->connection->GlobalID;
			}
		}
	}

	void NetworkManager::Update(Scene* context)
	{
		m_Context = context;
		auto& Hosts = m_Context->GetAllEntitiesWith<NetworkHostComponent>();
		if (Hosts.empty() == false)
		{
			for (auto& Host : Hosts)
			{
				Entity CurrEnt{ Host, m_Context };
				NetworkHostComponent* comp = &CurrEnt.GetComponent<NetworkHostComponent>();
				if (comp->server != nullptr)
					comp->server->ServerUpdate();
			}
		}

		auto& Clients = m_Context->GetAllEntitiesWith<NetworkClientComponent>();
		if (Clients.empty() == false)
		{
			for (auto& Client : Clients)
			{
				Entity CurrEnt{ Client, m_Context };
				NetworkClientComponent* comp = &CurrEnt.GetComponent<NetworkClientComponent>();
				comp->CheckRecieving();
				comp->globalID = comp->connection->GlobalID;
			}
		}
	}

	void NetworkManager::StopNetwork()
	{
		auto& Hosts = m_Context->GetAllEntitiesWith<NetworkHostComponent>();
		if (Hosts.empty() == false)
		{
			for (auto& Host : Hosts)
			{
				Entity CurrEnt{ Host, m_Context };
				CurrEnt.GetComponent<NetworkHostComponent>().server->StopServer();
			}
		}

		auto& Clients = m_Context->GetAllEntitiesWith<NetworkClientComponent>();
		if (Clients.empty() == false)
		{
			for (auto& Client : Clients)
			{
				Entity CurrEnt{ Client, m_Context };
				CurrEnt.GetComponent<NetworkClientComponent>().connection->Disconnect();
				CurrEnt.GetComponent<NetworkClientComponent>().globalID = 0;
			}
		}
	}
}