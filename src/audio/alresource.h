#pragma once

#include <al.h>

namespace TankGame
{
	template <void (*DEL)(ALuint)>
	class ALResource
	{
	public:
		using Deleter = void (*)(ALuint);
		
		inline ALResource() : m_isNull(true) { }
		
		inline explicit ALResource(ALuint id)
				: m_isNull(false), m_id(id) { }
		
		virtual ~ALResource()
		{
			if (!m_isNull)
				DEL(m_id);
		}
		
		inline ALResource(ALResource&& other)
		    : m_isNull(other.m_isNull), m_id(other.m_id)
		{
			other.m_isNull = true;
		}
		
		ALResource& operator=(ALResource&& other)
		{
			this->~GLResource();
			
			m_isNull = other.m_isNull;
			m_id = other.m_id;
			other.m_isNull = true;
			
			return *this;
		}
		
		ALResource(const ALResource& other) = delete;
		ALResource& operator=(const ALResource& other) = delete;
		
		bool operator==(decltype(nullptr)) const
		{ return m_isNull; }
		
		inline ALuint GetID() const
		{ return m_id; }
		
		void SetID(ALuint id)
		{
			if (!m_isNull)
				DEL(m_id);
			else
				m_isNull = false;
			
			m_id = id;
		}
		
		inline bool IsNull() const
		{ return m_isNull; }
		
	private:
		bool m_isNull;
		ALuint m_id;
	};
}
