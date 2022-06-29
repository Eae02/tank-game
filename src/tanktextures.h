#pragma once

#include "graphics/gl/texture2d.h"
#include "graphics/spritematerial.h"


namespace TankGame
{
	class TankTextures
	{
	public:
		inline const Texture2D& GetBaseDiffuse(int frame) const
		{ return m_baseDiffuse.at(frame); }
		inline const Texture2D& GetBaseNormals() const
		{ return m_baseNormals; }
		
		inline const SpriteMaterial& GetBaseMaterial(int frame) const
		{ return m_baseMaterials.at(frame); }
		
		static void LoadAndCreateInstance(class ASyncWorkList& asyncWorkList);
		static const TankTextures& GetInstance() { return *s_instance; }
		
	private:
		TankTextures(std::vector<Texture2D> baseDiffuse, Texture2D baseNormals);
		
		static std::unique_ptr<TankTextures> s_instance;
		
		std::vector<Texture2D> m_baseDiffuse;
		Texture2D m_baseNormals;
		
		std::vector<SpriteMaterial> m_baseMaterials;
	};
}
