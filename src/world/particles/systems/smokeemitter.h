#pragma once

#include "../particleemitter.h"
#include "../spherevec2generator.h"
#include "../../../graphics/gl/texture2darray.h"


namespace TankGame
{
	class SmokeEmitter : public ParticleEmitter
	{
	public:
		explicit SmokeEmitter(ParticlesManager& particlesManager);
		
		void SetTransformationProvider(const class ITransformationProvider* transformationProvider);
		
	protected:
		virtual glm::vec2 GeneratePosition(float subframeInterpolation) const override;
		virtual glm::vec2 GenerateVelocity(float subframeInterpolation) const override;
		
	private:
		static std::unique_ptr<Texture2DArray> s_smokeTexture;
		
		SphereVec2Generator m_positionGenerator;
		SphereVec2Generator m_velocityGenerator;
	};
}
