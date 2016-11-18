#pragma once

#include "../iasyncoperation.h"
#include "../utils/filesystem.h"
#include "tilegridmaterial.h"

#include <vector>

namespace TankGame
{
	class TileGridMaterialLoadOperation : public IASyncOperation
	{
	public:
		using DoneCallback = void(*)(TileGridMaterial&&);
		
		TileGridMaterialLoadOperation(fs::path jsonPath, DoneCallback doneCallback);
		
		virtual void DoWork() override;
		virtual void ProcessResult() override;
		
	private:
		using TextureLayerPtr = std::unique_ptr<uint8_t, void(*)(uint8_t*)>;
		
		static TextureLayerPtr CreateLayerPtr();
		TextureLayerPtr LoadTextureLayer(const fs::path& path);
		
		DoneCallback m_doneCallback;
		fs::path m_jsonPath;
		
		int m_layers = 0;
		
		std::vector<std::string> m_materialNames;
		std::vector<TileGridMaterial::MaterialSettings> m_materialSettings;
		std::vector<bool> m_isSolid;
		
		std::vector<TextureLayerPtr> m_diffuseLayers;
		std::vector<TextureLayerPtr> m_normalMapLayers;
		std::vector<TextureLayerPtr> m_specularMapLayers;
	};
}
