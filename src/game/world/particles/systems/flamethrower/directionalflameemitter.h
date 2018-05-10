#pragma once

#include "../explosionflameemitter.h"
#include "../../particleemitter.h"
#include "../../spherevec2generator.h"
#include "../../../../graphics/gl/texture2darray.h"

#include <memory>

namespace TankGame
{
	class DirectionalFlameEmitter : public ExplosionFlameEmitter
	{
	public:
		explicit DirectionalFlameEmitter(float flameLength, ParticlesManager& particlesManager);
		
		void SetTransformationProvider(const class ITransformationProvider* transformationProvider);
		
		static float GetHalfEmitAngle(float flameLength);
		
		static constexpr float FireSpeed = 4.5f;
		
	protected:
		virtual glm::vec2 GeneratePosition(float subframeInterpolation) const override;
		virtual glm::vec2 GenerateVelocity(float subframeInterpolation) const override;
		
	private:
		static std::unique_ptr<Texture2DArray> s_flameTexture;
		
		SphereVec2Generator m_positionGenerator;
		SphereVec2Generator m_velocityGenerator;
	};
}
