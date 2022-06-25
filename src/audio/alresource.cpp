#include "alresource.h"

namespace TankGame
{
	ALResource& ALResource::operator=(ALResource&& other)
	{
		this->~ALResource();
		
		m_deleter = other.m_deleter;
		m_id = other.m_id;
		other.m_deleter = nullptr;
		other.m_id = UINT32_MAX;
		
		return *this;
	}
}
