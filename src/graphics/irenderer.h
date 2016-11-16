#pragma once

#include "../utils/abstract.h"

namespace TankGame
{
	class IRenderer : public Abstract
	{
	public:
		virtual void DrawGeometry(const class ViewInfo& viewInfo) const = 0;
		virtual void DrawTranslucentGeometry(const class ViewInfo& viewInfo) const = 0;
		
		virtual void DrawDistortions(const class ViewInfo& viewInfo) const = 0;
		virtual void DrawLighting(const class ViewInfo& viewInfo) const = 0;
		virtual void DrawParticles(const class ViewInfo& viewInfo, class ParticleRenderer& renderer) const = 0;
	};
}
