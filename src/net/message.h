#pragma once

#include <vector>
#include <cstdint>

namespace TankGame
{
	enum class MessageID : uint8_t
	{
		
	};
	
	class Message
	{
	public:
		Message() = default;
		
		inline Message(MessageID id, size_t size)
			: m_id(id), m_data(size) { }
		
		inline const char* GetData() const
		{
			return m_data.data();
		}
		
		inline char* GetData()
		{
			return m_data.data();
		}
		
		inline size_t GetSize() const
		{
			return m_data.size();
		}
		
		inline MessageID GetID() const
		{
			return m_id;
		}
		
	private:
		MessageID m_id;
		std::vector<char> m_data;
	};
}
