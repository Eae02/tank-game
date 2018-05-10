#pragma once

namespace TankGame
{
	class Attenuation
	{
	public:
		inline Attenuation()
		    : m_linear(0.0f), m_exponent(1.0f) { }
		
		inline Attenuation(float linear, float exponent)
		    : m_linear(linear), m_exponent(exponent) { }
		
		inline float GetLinear() const
		{ return m_linear; }
		inline void SetLinear(float linear)
		{ m_linear = linear; }
		
		inline float GetExponent() const
		{ return m_exponent; }
		inline void SetExponent(float exponent)
		{ m_exponent = exponent; }
	
	private:
		float m_linear;
		float m_exponent;
	};
}
