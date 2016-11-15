#pragma once

#include "graphics/gl/texture2d.h"
#include "graphics/spritematerial.h"
#include <memory>

namespace TankGame
{
	class TankTextures
	{
	public:
		TankTextures();
		
		inline const Texture2D& GetBaseDiffuse(int frame) const
		{ return m_baseDiffuse[frame]; }
		inline const Texture2D& GetBaseNormals() const
		{ return m_baseNormals; }
		
		inline const SpriteMaterial& GetBaseMaterial(int frame) const
		{ return m_baseMaterials[frame]; }
		
		inline static const TankTextures& GetInstance()
		{ return *s_instance; }
		inline static void SetInstance(std::unique_ptr<TankTextures>&& instance)
		{ s_instance = std::move(instance); }
		
	private:
		static std::unique_ptr<TankTextures> s_instance;
		
		Texture2D m_baseDiffuse[9];
		Texture2D m_baseNormals;
		
		SpriteMaterial m_baseMaterials[9];
	};
}
