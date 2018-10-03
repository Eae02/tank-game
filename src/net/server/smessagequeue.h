#pragma once

#include "../message.h"

#include <queue>
#include <mutex>

namespace TankGame
{
	class SMessageQueue
	{
	public:
		SMessageQueue() = default;
		
		inline void Enqueue(Message message)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_messages.push(std::move(message));
		}
		
		inline Message Dequeue()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_messages.empty())
				return { };
			Message message = m_messages.front();
			m_messages.pop();
			return message;
		}
		
	private:
		std::mutex m_mutex;
		std::queue<Message> m_messages;
	};
}
