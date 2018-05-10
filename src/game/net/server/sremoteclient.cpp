#include "sremoteclient.h"
#include "../protocol.h"

namespace TankGame
{
	SRemoteClient::SRemoteClient(TCPSocket socket, std::string name, uint16_t id)
		: m_socket(std::move(socket)), m_name(std::move(name)), m_id(id),
		  m_thread(&SRemoteClient::ThreadTarget, this) { }
	
	void SRemoteClient::ThreadTarget()
	{
		size_t messagePos = 0;
		Message currentMessage;
		
		while (true)
		{
			int64_t size = m_socket.Read(m_readBuffer.data(), m_readBuffer.size());
			if (size == 0)
				break;
			
			const char* availDataBegin;
			size_t availData;
			
			if (messagePos > 0)
			{
				availDataBegin = m_readBuffer.data();
				availData = static_cast<size_t>(size);
			}
			else
			{
				auto* header = reinterpret_cast<MessageHeader*>(m_readBuffer.data());
				
				messagePos = 0;
				currentMessage = Message(header->id, header->size);
				
				availData = size - sizeof(MessageHeader);
				availDataBegin = m_readBuffer.data() + sizeof(MessageHeader);
			}
			
			size_t bytesToCopy = std::min(availData, currentMessage.GetSize() - messagePos);
			std::copy_n(availDataBegin, bytesToCopy, currentMessage.GetData() + messagePos);
			messagePos += bytesToCopy;
			
			if (messagePos >= currentMessage.GetSize())
			{
				m_messageQueue->Enqueue(std::move(currentMessage));
				messagePos = 0;
			}
		}
	}
}
