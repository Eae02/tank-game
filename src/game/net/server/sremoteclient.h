#pragma once

#include <thread>
#include <string>

#include "../tcpsocket.h"
#include "../connectionresponse.h"
#include "smessagequeue.h"

namespace TankGame
{
	class SRemoteClient
	{
	public:
		SRemoteClient(TCPSocket socket, std::string name, uint16_t id);
		
		const std::string& GetName() const
		{
			return m_name;
		}
		
		inline uint16_t GetID() const
		{
			return m_id;
		}
		
	private:
		void ThreadTarget();
		
		SMessageQueue* m_messageQueue;
		
		std::array<char, 1024> m_readBuffer;
		
		TCPSocket m_socket;
		
		std::string m_name;
		uint16_t m_id;
		
		std::thread m_thread;
	};
}
