#pragma once

#include "gl/texture2darray.h"
#include "gl/bufferallocator.h"
#include "gl/shaderprogram.h"
#include "../utils/memory/stackobject.h"
#include "../utils/filesystem.h"

#include <glm/vec2.hpp>
#include <algorithm>

namespace TankGame
{
	class TileGridMaterial
	{
		friend class TileGridMaterialLoadOperation;
		
	public:
		static TileGridMaterial FromFile(const fs::path& path);
		
		void Bind() const;
		void BindForShadowRender() const;
		
		void SetMaterialSpecularIntensity(uint8_t material, float specularIntensity)
		{
			m_materialSettings[material].m_specularIntensity = specularIntensity;
			m_parametersNeedUpload = true;
		}
		
		const std::string& GetMaterialName(uint8_t id) const;
		
		inline bool IsSolid(uint8_t id) const
		{ return m_isSolid[id]; }
		
		inline int GetMaterialCount() const
		{ return m_layerCount; }
		
		static inline TileGridMaterial& GetInstance()
		{ return *s_instance; }
		static inline void SetInstance(std::unique_ptr<TileGridMaterial>&& instance)
		{ s_instance = std::move(instance); }
		
		static constexpr GLsizei WIDTH = 512;
		static constexpr GLsizei HEIGHT = 512;
		
	private:
		struct MaterialSettings
		{
			glm::vec2 m_textureScale = { 1.0f, 1.0f };
			float m_specularIntensity = 1.0f;
			float m_specularExponent = 20.0f;
		};
		
		static const int MIPMAP_COUNT;
		
		static std::unique_ptr<TileGridMaterial> s_instance;
		
		explicit TileGridMaterial(int layerCount);
		
		int m_layerCount;
		
		Texture2DArray m_diffuseTextureArray;
		Texture2DArray m_normalMapTextureArray;
		Texture2DArray m_specularMapTextureArray;
		
		MaterialSettings m_materialSettings[256];
		
		bool m_isSolid[256];
		
		std::vector<std::string> m_materialNames;
		
		StackObject<Buffer> m_isSolidBuffer;
		
		BufferAllocator::UniquePtr m_parametersBuffer;
		mutable bool m_parametersNeedUpload = true;
		
		ShaderProgram m_shader;
	};
}
