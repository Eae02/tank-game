#include "rectanglevec2generator.h"
#include "../itransformationprovider.h"
#include "../../transform.h"
#include "../../utils/mathutils.h"

namespace TankGame
{
	glm::vec2 RectangleVec2Generator::GenerateVec2(std::mt19937& random, float subframeInterpolation) const
	{
		glm::vec2 relativeVector(m_xDistribution(random), m_yDistribution(random));
		
		if (m_relativeTransformProvider != nullptr)
			relativeVector = RotateVector(relativeVector, m_relativeTransformProvider->GetTransform().GetRotation());
		
		return relativeVector + GetOffset(subframeInterpolation);
	}
	
	void RectangleVec2Generator::SetRelativeTransformProvider(const ITransformationProvider* transformProvider, bool offset)
	{
		m_relativeTransformProvider = transformProvider;
		m_relativeTransformLFProvider = dynamic_cast<const ILastFrameTransformProvider*>(transformProvider);
		m_offsetByRelativeTransform = offset;
	}
	
	glm::vec2 RectangleVec2Generator::GetOffset(float subframeInterpolation) const
	{
		if (m_relativeTransformProvider == nullptr || !m_offsetByRelativeTransform)
			return glm::vec2(0.0f);
		if (m_relativeTransformLFProvider == nullptr)
			return m_relativeTransformProvider->GetTransform().GetPosition();
		
		return glm::mix(m_relativeTransformLFProvider->GetLastFrameTransform().GetPosition(),
		                m_relativeTransformProvider->GetTransform().GetPosition(), subframeInterpolation);
	}
}
