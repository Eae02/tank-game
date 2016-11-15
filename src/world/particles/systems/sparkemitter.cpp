#include "sparkemitter.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	StackObject<Texture2DArray> SparkEmitter::s_sparkTextureArray;
	
	SparkEmitter::SparkEmitter(ParticlesManager& particlesManager, float maxAngleDivergence, float speedMul)
	    : ParticleEmitter(particlesManager), m_positionGenerator(0, glm::two_pi<float>(), 0.05f, 0.1f),
	      m_velocityGenerator(-maxAngleDivergence, maxAngleDivergence, 3.0f, 4.0f)
	{
		if (s_sparkTextureArray.IsNull())
		{
			s_sparkTextureArray.Construct(128, 128 / 8, 1, Texture::GetMipmapCount(128), GL_RGBA8);
			s_sparkTextureArray->LoadLayerFromFile(0, GetResDirectory() / "particles" / "spark.png");
			s_sparkTextureArray->SetupMipmapping(true);
			
			CallOnClose([] { s_sparkTextureArray.Destroy(); });
		}
		
		SetTextureArray(*s_sparkTextureArray);
		
		SetUseAdditiveBlending(true);
		
		SetAlignParticlesToVelocity(true);
		
		SetSpawnRate(500);
		SetLifeTime(0.05f * speedMul, 0.2f * speedMul);
		
		SetBeginOpacity(0.7f, 1.0f);
		SetEndOpacity(0.0f);
		
		SetBeginSize(0.09f * 2.5f, 0.12f * 2.5f);
		SetEndSize(0.09f, 0.12f);
	}
	
	void SparkEmitter::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_positionGenerator.SetRelativeTransformProvider(transformationProvider, true);
		m_velocityGenerator.SetRelativeTransformProvider(transformationProvider, false);
	}
	
	glm::vec2 SparkEmitter::GeneratePosition(float subframeInterpolation) const
	{
		return m_positionGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
	
	glm::vec2 SparkEmitter::GenerateVelocity(float subframeInterpolation) const
	{
		return m_velocityGenerator.GenerateVec2(GetRandom(), subframeInterpolation);
	}
}
