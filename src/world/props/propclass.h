#pragma once

#include <memory>
#include "../../graphics/gl/texture2d.h"
#include "../../graphics/spritematerial.h"
#include "../../utils/filesystem.h"

namespace TankGame
{
	class PropClass
	{
	public:
		static PropClass FromJSON(const fs::path& path, class PropsManager& propsManager);
		
		inline const std::string& GetName() const
		{ return m_name; }
		
		inline bool IsDecal() const
		{ return m_isDecal; }
		
		inline GLsizei GetTextureWidth() const
		{ return m_diffuse->GetWidth(); }
		inline GLsizei GetTextureHeight() const
		{ return m_diffuse->GetHeight(); }
		
		inline const SpriteMaterial& GetMaterial() const
		{ return m_material; }
		
	private:
		inline explicit PropClass(SpriteMaterial spriteMaterial)
		    : m_material(std::move(spriteMaterial)) { }
		
		bool m_isDecal = false;
		
		std::shared_ptr<Texture2D> m_diffuse;
		std::shared_ptr<Texture2D> m_normalMap;
		
		SpriteMaterial m_material;
		
		std::string m_name;
	};
}
