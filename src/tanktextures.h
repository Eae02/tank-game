#pragma once

#include "graphics/gl/texture2d.h"
#include "graphics/spritematerial.h"
#include "iasyncoperation.h"

#include <memory>

namespace TankGame
{
	class TankTextures
	{
	public:
		TankTextures(Texture2D&& baseDiffuse0, Texture2D&& baseDiffuse1, Texture2D&& baseDiffuse2,
		             Texture2D&& baseDiffuse3, Texture2D&& baseDiffuse4, Texture2D&& baseDiffuse5,
		             Texture2D&& baseDiffuse6, Texture2D&& baseDiffuse7, Texture2D&& baseDiffuse8,
		             Texture2D&& baseNormals);
		
		inline const Texture2D& GetBaseDiffuse(int frame) const
		{ return m_baseDiffuse[frame]; }
		inline const Texture2D& GetBaseNormals() const
		{ return m_baseNormals; }
		
		inline const SpriteMaterial& GetBaseMaterial(int frame) const
		{ return m_baseMaterials[frame]; }
		
		static std::unique_ptr<IASyncOperation> CreateInstance();
		static const TankTextures& GetInstance();
		
	private:
		static std::unique_ptr<TankTextures> s_instance;
		
		Texture2D m_baseDiffuse[9];
		Texture2D m_baseNormals;
		
		SpriteMaterial m_baseMaterials[9];
	};
}
