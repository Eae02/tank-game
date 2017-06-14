#include "hittable.h"

namespace TankGame
{
	void Hittable::ChangeHp(float delta, HpSource source)
	{
		
	}
	
	void Hittable::SetHp(float hp)
	{
		m_hp = hp;
		
		if (m_hp > m_maxHp)
		{
			m_hp = m_maxHp;
		}
		else if (m_hp < 1)
		{
			m_hp = 0.0f;
			OnKilled();
		}
	}
}
