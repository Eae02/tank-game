#pragma once

#include "../graphics/worldrenderer.h"

namespace TankGame
{
	class EditorRenderer : public WorldRenderer
	{
	public:
		virtual void DrawLighting(const class ViewInfo& viewInfo) const override;
		
		inline bool UseGameLighting() const
		{ return m_useGameLighting; }
		inline void SetUseGameLighting(bool useGameLighting)
		{ m_useGameLighting = useGameLighting; }
		
	private:
		bool m_useGameLighting = false;
	};
}
