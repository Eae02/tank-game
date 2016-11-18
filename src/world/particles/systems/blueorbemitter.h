#pragma once

#include "../particleemitter.h"
#include "../spherevec2generator.h"
#include "../../../utils/memory/stackobject.h"
#include "../../../graphics/gl/texture2darray.h"

namespace TankGame
{
	class BlueOrbEmitter : public ParticleEmitter
	{
	public:
		explicit BlueOrbEmitter(ParticlesManager& particlesManager);
		
		void SetTransformationProvider(const class ITransformationProvider* transformationProvider);
		
	protected:
		virtual glm::vec2 GeneratePosition(float subframeInterpolation) const override;
		virtual glm::vec2 GenerateVelocity(float subframeInterpolation) const override;
		
	private:
		static StackObject<Texture2DArray> s_texture;
		
		SphereVec2Generator m_positionGenerator;
		SphereVec2Generator m_velocityGenerator;
	};
}