#pragma once

#include "ivec2generator.h"

namespace TankGame
{
	class SphereVec2Generator : public IVec2Generator
	{
	public:
		SphereVec2Generator();
		SphereVec2Generator(float angleMin, float angleMax, float radiusMin, float radiusMax);
		
		virtual glm::vec2 GenerateVec2(std::mt19937& random, float subframeInterpolation) const override;
		
		inline void SetAngleDistribution(float min, float max)
		{ m_angleDistribution = std::uniform_real_distribution<float>(min, max); }
		inline void SetRadiusDistribution(float min, float max)
		{ m_radiusDistribution = std::uniform_real_distribution<float>(min, max); }
		
		void SetRelativeTransformProvider(const class ITransformationProvider* transformProvider, bool offset = true);
		
	private:
		glm::vec2 GetOffset(float subframeInterpolation) const;
		
		mutable std::uniform_real_distribution<float> m_angleDistribution;
		mutable std::uniform_real_distribution<float> m_radiusDistribution;
		
		const class ITransformationProvider* m_relativeTransformProvider = nullptr;
		const class ILastFrameTransformProvider* m_relativeTransformLFProvider = nullptr;
		
		bool m_offsetByRelativeTransform = true;
	};
}
