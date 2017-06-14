#include "smokeemitter.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	std::unique_ptr<Texture2DArray> SmokeEmitter::s_smokeTexture;
	
	SmokeEmitter::SmokeEmitter(ParticlesManager& particlesManager)
	    : ParticleEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0, 0.3f),
	      m_velocityGenerator(0, glm::two_pi<float>(), 0.1f, 0.3f)
	{
		if (s_smokeTexture == nullptr)
		{
			s_smokeTexture = std::make_unique<Texture2DArray>(512, 512, 1, Texture::GetMipmapCount(512), GL_RGBA8);
			s_smokeTexture->LoadLayerFromFile(0, GetResDirectory() / "particles" / "smoke.png");
			s_smokeTexture->SetupMipmapping(true);
			
			CallOnClose([] { s_smokeTexture = nullptr; });
		}
		
		SetTextureArray(*s_smokeTexture);
		
		SetUseAdditiveBlending(false);
		
		SetSpawnRate(400);
		SetLifeTime(0.3, 0.5);
		
		SetBeginOpacity(0.5f, 0.5f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.25f, 0.35f);
		SetEndSize(0.05f, 0.15f);
	}
	
	void SmokeEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	glm::vec2 SmokeEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 SmokeEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
}
