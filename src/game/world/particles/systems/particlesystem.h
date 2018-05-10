#pragma once

#include "../../../utils/abstract.h"
#include "../../../transform.h"
#include "../../itransformationprovider.h"

#include <cstddef>

namespace TankGame
{
	class IParticleSystem : public Abstract
	{
	public:
		virtual class ParticleEmitter* GetEmitter(size_t n) = 0;
		virtual size_t GetEmitterCount() const = 0;
		virtual void SetTransformationProvider(const ITransformationProvider* transformationProvider) = 0;
	};
}
