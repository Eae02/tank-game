#pragma once

#include "../../circle.h"

namespace TankGame
{
	class Hittable
	{
	public:
		explicit inline Hittable(float maxHp, int teamID)
			: m_hp(maxHp), m_maxHp(maxHp), m_teamID(teamID) { }
		
		inline float GetHp() const
		{ return m_hp; }
		inline float GetMaxHp() const
		{ return m_maxHp; }
		
		inline int GetTeamID() const
		{ return m_teamID; }
		
		virtual void SetHp(float hp);
		
		virtual Circle GetHitCircle() const = 0;
		
	protected:
		virtual void OnKilled() { }
		
	private:
		float m_hp;
		float m_maxHp;
		int m_teamID;
	};
}
