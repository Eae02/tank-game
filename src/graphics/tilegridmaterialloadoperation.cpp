#include "tilegridmaterialloadoperation.h"
#include "textureloadoperation.h"
#include "../utils/jsonparseutils.h"
#include "../utils/ioutils.h"

#include <nlohmann/json.hpp>
#include <stb_image.h>
#include <stb_image_resize.h>

namespace TankGame
{
	TileGridMaterialLoadOperation::TileGridMaterialLoadOperation(fs::path jsonPath,
	      TileGridMaterialLoadOperation::DoneCallback doneCallback)
	    : m_doneCallback(doneCallback), m_jsonPath(std::move(jsonPath))
	{
		
	}
	
	constexpr size_t BYTES_PER_LAYER = TileGridMaterial::WIDTH * TileGridMaterial::HEIGHT * 4;
	
	void TileGridMaterialLoadOperation::DoWork()
	{
		nlohmann::json jsonDocument = nlohmann::json::parse(ReadFileContents(m_jsonPath));
		
		std::vector<std::string> diffusePaths;
		std::vector<std::string> normalMapPaths;
		std::vector<std::string> specularMapPaths;
		
		for (const nlohmann::json& tileElement : jsonDocument)
		{
			int id = tileElement["id"].get<int>();
			if (id < 0 || id > 255)
				throw std::runtime_error("Tile id out of range (" + std::to_string(id) + ").");
			
			if (id >= m_layers)
			{
				m_layers = id + 1;
				diffusePaths.resize(m_layers);
				normalMapPaths.resize(m_layers);
				specularMapPaths.resize(m_layers);
				m_materialNames.resize(m_layers);
				m_materialSettings.resize(m_layers);
				m_isSolid.resize(m_layers);
			}
			
			auto solidIt = tileElement.find("solid");
			if (solidIt == tileElement.end() || !solidIt->is_boolean())
				m_isSolid[id] = false;
			else
				m_isSolid[id] = solidIt->get<bool>();
			
			auto nameIt = tileElement.find("name");
			if (nameIt != tileElement.end())
				m_materialNames[id] = nameIt->get<std::string>();
			else
				m_materialNames[id] = "Unnamed material";;
			
			auto diffuseIt = tileElement.find("diffuse");
			auto normalMapIt = tileElement.find("normalMap");
			auto specularMapIt = tileElement.find("specularMap");
			
			//Reads texture paths
			if (diffuseIt == tileElement.end())
				continue;
			diffusePaths[id] = diffuseIt->get<std::string>();
			if (normalMapIt != tileElement.end())
				normalMapPaths[id] = normalMapIt->get<std::string>();
			if (specularMapIt != tileElement.end())
				specularMapPaths[id] = specularMapIt->get<std::string>();
			
			//Reads specular settings
			auto specularIt = tileElement.find("specular");
			if (specularIt != tileElement.end())
			{
				m_materialSettings[id].m_specularIntensity = (*specularIt)["intensity"].get<float>();
				m_materialSettings[id].m_specularExponent = (*specularIt)["exponent"].get<float>();
			}
			
			//Reads texture scale settings
			auto textureScaleIt = tileElement.find("textureScale");
			if (textureScaleIt == tileElement.end())
				m_materialSettings[id].m_textureScale = glm::vec2(1.0f);
			else
				m_materialSettings[id].m_textureScale = ParseVec2(*textureScaleIt);
		}
		
		fs::path parentPath = m_jsonPath.parent_path();
		
		for (int i = 0; i < m_layers; i++)
		{
			if (diffusePaths[i].empty())
			{
				GetLogStream() << "[warning] No diffuse for tile with id " << i << '\n';
				auto layerData = AllocateTextureLayer();
				std::memset(layerData.get(), 0, BYTES_PER_LAYER);
				m_diffuseLayers.push_back(std::move(layerData));
			}
			else
			{
				m_diffuseLayers.emplace_back(LoadTextureLayer(parentPath / diffusePaths[i]));
			}
			
			TextureLayerPtr normalSpecData;
			
			if (!normalMapPaths[i].empty())
			{
				normalSpecData = LoadTextureLayer(parentPath / normalMapPaths[i]);
			}
			else
			{
				normalSpecData = AllocateTextureLayer();
				std::memset(normalSpecData.get(), 0x0000FFFF, BYTES_PER_LAYER);
			}
			
			if (!specularMapPaths[i].empty())
			{
				TextureLayerPtr specularData = LoadTextureLayer(parentPath / specularMapPaths[i]);
				for (size_t i = 0; i < BYTES_PER_LAYER; i += 4)
					normalSpecData.get()[i + 3] = specularData.get()[i];
			}
			
			m_normalSpecLayers.push_back(std::move(normalSpecData));
		}
	}
	
	void TileGridMaterialLoadOperation::ProcessResult()
	{
		TileGridMaterial material(m_layers);
		
		material.m_materialNames = std::move(m_materialNames);
		
		for (int i = 0; i < m_layers; i++)
		{
			glTextureSubImage3D(material.m_diffuseTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
			                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_diffuseLayers[i].get());
			
			glTextureSubImage3D(material.m_normalSpecTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
			                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_normalSpecLayers[i].get());
		}
		
		std::copy(m_materialSettings.begin(), m_materialSettings.end(), material.m_materialSettings);
		std::copy(m_isSolid.begin(), m_isSolid.end(), material.m_isSolid);
		
		material.m_diffuseTextureArray.SetupMipmapping(true);
		material.m_normalSpecTextureArray.SetupMipmapping(true);
		
		m_doneCallback(std::move(material));
	}
	
	TileGridMaterialLoadOperation::TextureLayerPtr TileGridMaterialLoadOperation::AllocateTextureLayer()
	{
		return TextureLayerPtr(reinterpret_cast<uint8_t*>(malloc(BYTES_PER_LAYER)));
	}
	
	TileGridMaterialLoadOperation::TextureLayerPtr TileGridMaterialLoadOperation::LoadTextureLayer(const fs::path& path)
	{
		int width, height;
		
		std::string pathString = path.string();
		
		TextureLayerPtr layerData(stbi_load(pathString.c_str(), &width, &height, nullptr, 4));
		
		if (width == TileGridMaterial::WIDTH && height == TileGridMaterial::HEIGHT)
			return layerData;
		
		TextureLayerPtr resizedData = AllocateTextureLayer();
		
		stbir_resize_uint8(layerData.get(), width, height, 0, resizedData.get(),
		                   TileGridMaterial::WIDTH, TileGridMaterial::HEIGHT, 0, 4);
		
		return resizedData;
	}
}
