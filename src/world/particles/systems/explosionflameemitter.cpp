#include "explosionflameemitter.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/utils.h"


namespace TankGame
{
	std::unique_ptr<Texture2DArray> ExplosionFlameEmitter::s_flameTexture;
	
	ExplosionFlameEmitter::ExplosionFlameEmitter(ParticlesManager& particlesManager)
	    : ParticleEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0, 0.4f),
	      m_velocityGenerator(0, glm::two_pi<float>(), 0, 1.0f)
	{
		if (s_flameTexture== nullptr)
		{
			s_flameTexture = std::make_unique<Texture2DArray>(512, 512, 1, Texture::GetMipmapCount(512), TextureFormat::RGBA8);
			s_flameTexture->LoadLayerFromFile(0, resDirectoryPath / "particles" / "flame.png");
			s_flameTexture->SetupMipmapping(true);
			
			CallOnClose([] { s_flameTexture = nullptr; });
		}
		
		SetTextureArray(*s_flameTexture);
		
		SetUseAdditiveBlending(true);
		
		SetSpawnRate(800);
		SetLifeTime(0.4f, 0.5f);
		
		SetBeginOpacity(0.7f, 1.0f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.5f, 0.7f);
		SetEndSize(0.4f, 0.6f);
	}
	
	void ExplosionFlameEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	glm::vec2 ExplosionFlameEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 ExplosionFlameEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
}
