#pragma once

#include "ivec2generator.h"

namespace TankGame
{
	class RectangleVec2Generator : public IVec2Generator
	{
	public:
		RectangleVec2Generator(glm::vec2 min, glm::vec2 max)
		    : m_xDistribution(min.x, max.x), m_yDistribution(min.y, max.y) { }
		
		virtual glm::vec2 GenerateVec2(std::mt19937& random, float subframeInterpolation) const override;
		
		void SetRelativeTransformProvider(const class ITransformationProvider* transformProvider, bool offset = true);
		
	private:
		glm::vec2 GetOffset(float subframeInterpolation) const;
		
		mutable std::uniform_real_distribution<float> m_xDistribution;
		mutable std::uniform_real_distribution<float> m_yDistribution;
		
		const class ITransformationProvider* m_relativeTransformProvider = nullptr;
		const class ILastFrameTransformProvider* m_relativeTransformLFProvider = nullptr;
		
		bool m_offsetByRelativeTransform = true;
	};
}
