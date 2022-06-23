#include "energyplasmaemitter.h"
#include "../../../utils/utils.h"
#include "../../../utils/ioutils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	std::unique_ptr<Texture2DArray> EnergyPlasmaEmitter::s_texture;
	
	EnergyPlasmaEmitter::EnergyPlasmaEmitter(ParticlesManager& particlesManager)
	    : ParticleEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0, 0.005f),
	      m_velocityGenerator(0, glm::two_pi<float>(), 0.0f, 0.05f)
	{
		if (s_texture== nullptr)
		{
			s_texture = std::make_unique<Texture2DArray>(85, 85, 1, Texture::GetMipmapCount(85), TextureFormat::RGBA8);
			s_texture->LoadLayerFromFile(0, resDirectoryPath / "particles" / "plasma_orb.png");
			s_texture->SetupMipmapping(true);
			
			CallOnClose([] { s_texture = nullptr; });
		}
		
		SetTextureArray(*s_texture);
		
		SetUseAdditiveBlending(true);
		
		SetSpawnRate(200);
		SetLifeTime(0.05f, 0.1f);
		
		SetBeginOpacity(1.0f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.2f, 0.25f);
		SetEndSize(0.15f, 0.2f);
	}
	
	void EnergyPlasmaEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	glm::vec2 EnergyPlasmaEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 EnergyPlasmaEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
}
