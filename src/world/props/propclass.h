#pragma once

#include "../../graphics/gl/texture2d.h"
#include "../../graphics/spritematerial.h"
#include "../../utils/filesystem.h"

#include <json.hpp>

namespace TankGame
{
	class PropClass
	{
	public:
		static PropClass FromJSON(const nlohmann::json& json, const fs::path& parentPath,
		                          class PropsManager& propsManager);
		
		inline const std::string& GetName() const
		{ return m_name; }
		
		inline bool IsDecal() const
		{ return m_isDecal; }
		
		inline GLsizei GetTextureWidth() const
		{ return m_textureWidth; }
		inline GLsizei GetTextureHeight() const
		{ return m_textureHeight; }
		
		inline const SpriteMaterial& GetMaterial() const
		{ return m_material; }
		
	private:
		inline explicit PropClass(SpriteMaterial spriteMaterial)
		    : m_material(std::move(spriteMaterial)) { }
		
		bool m_isDecal = false;
		
		GLsizei m_textureWidth;
		GLsizei m_textureHeight;
		
		SpriteMaterial m_material;
		
		std::string m_name;
	};
}
