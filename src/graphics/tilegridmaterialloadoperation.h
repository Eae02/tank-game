#pragma once

#include "../utils/filesystem.h"
#include "../utils/utils.h"
#include "tilegridmaterial.h"

#include <vector>
#include <future>

namespace TankGame
{
	class TileGridMaterialLoadOperation
	{
	public:
		static std::future<TileGridMaterialLoadOperation> Load(fs::path jsonPath);
		
		std::unique_ptr<TileGridMaterial> FinishLoading();
		
	private:
		TileGridMaterialLoadOperation() = default;
		
		using TextureLayerPtr = std::unique_ptr<uint8_t, FreeDeleter>;
		
		static TextureLayerPtr AllocateTextureLayer();
		TextureLayerPtr LoadTextureLayer(const fs::path& path);
		
		int m_layers = 0;
		
		std::vector<std::string> m_materialNames;
		std::vector<TileGridMaterial::MaterialSettings> m_materialSettings;
		std::vector<bool> m_isSolid;
		
		std::vector<TextureLayerPtr> m_diffuseLayers;
		std::vector<TextureLayerPtr> m_normalSpecLayers;
	};
}
