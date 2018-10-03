#include "server.h"
#include "../tcpsocket.h"
#include "../protocol.h"
#include "sremoteclient.h"

#include <thread>
#include <vector>
#include <sstream>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

namespace TankGame
{
	std::mutex g_clientsMutex;
	std::vector<std::unique_ptr<SRemoteClient>> g_clients;
	uint16_t g_nextClientID;
	
	SMessageQueue g_messageQueue;
	
	int maxClients = 8;
	
	static void AcceptThreadTarget()
	{
		TCPSocket socket = TCPSocket::CreateServer(SERVER_PORT);
		
		std::cout << "Listening on port " << SERVER_PORT << "..." << std::endl;
		
		std::array<char, 1024> initialDataBuffer;
		
		while (true)
		{
			TCPSocket connSocket = socket.Accept();
			
			connSocket.SetReceiveTimeout(std::chrono::seconds(3));
			int64_t size = connSocket.Read(initialDataBuffer.data(), initialDataBuffer.size());
			if (size == -1)
				continue;
			
			uint32_t nameLen = *reinterpret_cast<uint32_t*>(&initialDataBuffer[0]);
			if (nameLen > MAX_NAME_LEN || nameLen + 4 > size)
				continue;
			
			std::string name(&initialDataBuffer[4], nameLen);
			
			ConnectionResponse response = ConnectionResponse::OK;
			
			if (g_clients.size() > maxClients)
			{
				response = ConnectionResponse::ServerFull;
			}
			else
			{
				bool nameTaken = std::any_of(g_clients.begin(), g_clients.end(), [&] (const auto& client)
				{
					return client->GetName() == name;
				});
				
				if (nameTaken)
				{
					response = ConnectionResponse::NameTaken;
				}
			}
			
			connSocket.ClearReceiveTimeout();
			
			//Prepares the response message
			std::ostringstream responseStream;
			responseStream.write(reinterpret_cast<char*>(&response), sizeof(response));
			
			uint16_t id;
			if (response == ConnectionResponse::OK)
			{
				id = g_nextClientID++;
				responseStream.write(reinterpret_cast<char*>(&id), sizeof(id));
				
				//Writes information about other clients
				for (const std::unique_ptr<SRemoteClient>& client : g_clients)
				{
					uint16_t clientId = client->GetID();
					responseStream.write(reinterpret_cast<char*>(&clientId), sizeof(clientId));
					
					auto clientNameLen = static_cast<uint32_t>(client->GetName().size());
					responseStream.write(reinterpret_cast<char*>(&clientNameLen), sizeof(clientNameLen));
					
					responseStream.write(client->GetName().data(), clientNameLen);
				}
			}
			
			//Sends the response message
			std::string responseString = responseStream.str();
			connSocket.Write(responseString.data(), responseString.size());
			
			if (response == ConnectionResponse::OK)
			{
				std::cout << "'" << name << "' connected" << std::endl;
				
				auto client = std::make_unique<SRemoteClient>(std::move(connSocket), std::move(name), id);
				std::lock_guard<std::mutex> lock(g_clientsMutex);
				g_clients.push_back(std::move(client));
			}
		}
	}
	
	void StartServer()
	{
		g_nextClientID = 0;
		
		std::thread acceptThread(AcceptThreadTarget);
		
		while (true)
		{
			std::string command;
			std::getline(std::cin, command);
			
			size_t commandNameLen = command.find(' ');
			if (commandNameLen == std::string::npos)
				commandNameLen = command.size();
			
			if (command.compare(0, commandNameLen, "quit") == 0)
				std::exit(0);
			
			if (command.compare(0, commandNameLen, "start") == 0)
			{
				
			}
		}
	}
}

#pragma clang diagnostic pop
