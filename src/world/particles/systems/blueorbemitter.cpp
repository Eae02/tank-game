#include "blueorbemitter.h"
#include "../../../utils/utils.h"
#include "../../../utils/ioutils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	std::unique_ptr<Texture2DArray> BlueOrbEmitter::s_texture;
	
	BlueOrbEmitter::BlueOrbEmitter(ParticlesManager& particlesManager)
	    : ParticleEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0, 0.2f),
	      m_velocityGenerator(0, glm::two_pi<float>(), 0.0f, 0.3f)
	{
		if (s_texture== nullptr)
		{
			s_texture = std::make_unique<Texture2DArray>(64, 64, 1, Texture::GetMipmapCount(64), TextureFormat::RGBA8);
			s_texture->LoadLayerFromFile(0, resDirectoryPath / "particles" / "blue_orb.png");
			s_texture->SetupMipmapping(true);
			
			CallOnClose([] { s_texture = nullptr; });
		}
		
		SetTextureArray(*s_texture);
		
		SetUseAdditiveBlending(true);
		
		SetSpawnRate(1000);
		SetLifeTime(0.2f, 0.3f);
		
		SetBeginOpacity(0.7f, 1.0f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.05f, 0.1f);
		SetEndSize(0.02f, 0.05f);
	}
	
	void BlueOrbEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	glm::vec2 BlueOrbEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 BlueOrbEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
}
