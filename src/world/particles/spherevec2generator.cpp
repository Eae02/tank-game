#include "spherevec2generator.h"
#include "../itransformationprovider.h"
#include "../../transform.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	SphereVec2Generator::SphereVec2Generator()
	    : SphereVec2Generator(0.0f, glm::two_pi<float>(), 0.0f, 1.0f) { }
	
	SphereVec2Generator::SphereVec2Generator(float angleMin, float angleMax, float radiusMin, float radiusMax)
	    : m_angleDistribution(angleMin, angleMax), m_radiusDistribution(radiusMin, radiusMax) { }
	
	glm::vec2 SphereVec2Generator::GenerateVec2(std::mt19937& random, float subframeInterpolation) const
	{
		float theta = m_angleDistribution(random);
		float radius = m_radiusDistribution(random);
		
		if (m_relativeTransformProvider != nullptr)
			theta += m_relativeTransformProvider->GetTransform().GetRotation();
		
		return glm::vec2(std::cos(theta) * radius, std::sin(theta) * radius) + GetOffset(subframeInterpolation);
	}
	
	void SphereVec2Generator::SetRelativeTransformProvider(const ITransformationProvider* transformProvider, bool offset)
	{
		m_relativeTransformProvider = transformProvider;
		m_relativeTransformLFProvider = dynamic_cast<const ILastFrameTransformProvider*>(transformProvider);
		m_offsetByRelativeTransform = offset;
	}
	
	glm::vec2 SphereVec2Generator::GetOffset(float subframeInterpolation) const
	{
		if (m_relativeTransformProvider == nullptr || !m_offsetByRelativeTransform)
			return glm::vec2(0.0f);
		if (m_relativeTransformLFProvider == nullptr)
			return m_relativeTransformProvider->GetTransform().GetPosition();
		
		return glm::mix(m_relativeTransformLFProvider->GetLastFrameTransform().GetPosition(),
		                m_relativeTransformProvider->GetTransform().GetPosition(), subframeInterpolation);
	}
}
