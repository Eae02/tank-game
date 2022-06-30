#pragma once

namespace TankGame
{
	class ALResource
	{
	public:
		using Deleter = void (*)(int, const uint32_t*);
		
		inline ALResource() : m_deleter(nullptr), m_id(UINT32_MAX) { }
		
		inline ALResource(uint32_t id, Deleter deleter)
				: m_deleter(deleter), m_id(id) { }
		
		virtual ~ALResource()
		{
			if (m_deleter)
				m_deleter(1, &m_id);
		}
		
		inline ALResource(ALResource&& other)
		    : m_deleter(other.m_deleter), m_id(other.m_id)
		{
			other.m_deleter = nullptr;
			other.m_id = UINT32_MAX;
		}
		
		ALResource& operator=(ALResource&& other);
		
		ALResource(const ALResource& other) = delete;
		ALResource& operator=(const ALResource& other) = delete;
		
		bool operator==(decltype(nullptr)) const
		{ return m_deleter == nullptr; }
		
		inline uint32_t GetID() const
		{ return m_id; }
		
		inline bool IsNull() const
		{ return m_deleter != nullptr; }
		
	private:
		Deleter m_deleter;
		uint32_t m_id;
	};
}
