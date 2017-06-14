#include "directionalflameemitter.h"
#include "../../../../utils/ioutils.h"
#include "../../../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	constexpr float DirectionalFlameEmitter::FireSpeed;
	
	DirectionalFlameEmitter::DirectionalFlameEmitter(float flameLength, ParticlesManager& particlesManager)
	    : ExplosionFlameEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0, 0.05f),
	      m_velocityGenerator(0, 0, FireSpeed * 0.95f, FireSpeed * 1.05f)
	{
		float halfEmitAngle = GetHalfEmitAngle(flameLength);
		
		m_velocityGenerator.SetAngleDistribution(-glm::half_pi<float>() - halfEmitAngle,
		                                         -glm::half_pi<float>() + halfEmitAngle);
		
		double avgLifeTime = static_cast<double>(flameLength / FireSpeed);
		
		SetSpawnRate(100 * flameLength);
		SetLifeTime(avgLifeTime * 0.95, avgLifeTime * 1.05);
		
		SetBeginOpacity(0.7f, 1.0f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.2f, 0.25f);
		SetEndSize(0.45f, 0.5f);
	}
	
	void DirectionalFlameEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	float DirectionalFlameEmitter::GetHalfEmitAngle(float flameLength)
	{
		const float maxLengthDivergence = 1;
		return glm::atan((maxLengthDivergence / 2) / flameLength);
	}
	
	glm::vec2 DirectionalFlameEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 DirectionalFlameEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
}
