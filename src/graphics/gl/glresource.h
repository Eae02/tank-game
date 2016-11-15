#pragma once

#include <GL/glew.h>

namespace TankGame
{
	template <void (*DEL)(GLuint)>
	class GLResource
	{
	public:
		using Deleter = void (*)(GLuint);
		
		inline GLResource() : m_isNull(true) { }
		
		inline explicit GLResource(GLuint id)
				: m_isNull(false), m_id(id) { }
		
		virtual ~GLResource()
		{
			if (!m_isNull)
				DEL(m_id);
		}
		
		inline GLResource(GLResource&& other)
		    : m_isNull(other.m_isNull), m_id(other.m_id)
		{
			other.m_isNull = true;
		}
		
		GLResource& operator=(GLResource&& other)
		{
			this->~GLResource();
			
			m_isNull = other.m_isNull;
			m_id = other.m_id;
			other.m_isNull = true;
			
			return *this;
		}
		
		GLResource(const GLResource& other) = delete;
		GLResource& operator=(const GLResource& other) = delete;
		
		bool operator==(decltype(nullptr)) const
		{ return m_isNull; }
		
		inline GLuint GetID() const
		{ return m_id; }
		
		void SetID(GLuint id)
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
		GLuint m_id;
	};
}
