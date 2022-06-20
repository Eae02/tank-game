#include "tilegridmaterialloadoperation.h"
#include "textureloadoperation.h"
#include "../utils/jsonparseutils.h"
#include "../utils/ioutils.h"

#include <nlohmann/json.hpp>
#include <stb_image.h>
#include <stb_image_resize.h>

namespace TankGame
{
	constexpr size_t BYTES_PER_LAYER = TileGridMaterial::WIDTH * TileGridMaterial::HEIGHT * 4;
	
	std::future<TileGridMaterialLoadOperation> TileGridMaterialLoadOperation::Load(fs::path _jsonPath)
	{
		return std::async([jsonPath=std::move(_jsonPath)]
		{
			TileGridMaterialLoadOperation op;
			
			nlohmann::json jsonDocument = nlohmann::json::parse(ReadFileContents(jsonPath));
			
			std::vector<std::string> diffusePaths;
			std::vector<std::string> normalMapPaths;
			std::vector<std::string> specularMapPaths;
			
			for (const nlohmann::json& tileElement : jsonDocument)
			{
				int id = tileElement["id"].get<int>();
				if (id < 0 || id > 255)
					throw std::runtime_error("Tile id out of range (" + std::to_string(id) + ").");
				
				if (id >= op.m_layers)
				{
					op.m_layers = id + 1;
					diffusePaths.resize(op.m_layers);
					normalMapPaths.resize(op.m_layers);
					specularMapPaths.resize(op.m_layers);
					op.m_materialNames.resize(op.m_layers);
					op.m_materialSettings.resize(op.m_layers);
					op.m_isSolid.resize(op.m_layers);
				}
				
				auto solidIt = tileElement.find("solid");
				if (solidIt == tileElement.end() || !solidIt->is_boolean())
					op.m_isSolid[id] = false;
				else
					op.m_isSolid[id] = solidIt->get<bool>();
				
				auto nameIt = tileElement.find("name");
				if (nameIt != tileElement.end())
					op.m_materialNames[id] = nameIt->get<std::string>();
				else
					op.m_materialNames[id] = "Unnamed material";;
				
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
					op.m_materialSettings[id].m_specularIntensity = (*specularIt)["intensity"].get<float>();
					op.m_materialSettings[id].m_specularExponent = (*specularIt)["exponent"].get<float>();
				}
				
				//Reads texture scale settings
				auto textureScaleIt = tileElement.find("textureScale");
				if (textureScaleIt == tileElement.end())
					op.m_materialSettings[id].m_textureScale = glm::vec2(1.0f);
				else
					op.m_materialSettings[id].m_textureScale = ParseVec2(*textureScaleIt);
			}
			
			fs::path parentPath = jsonPath.parent_path();
			
			for (int i = 0; i < op.m_layers; i++)
			{
				if (diffusePaths[i].empty())
				{
					GetLogStream() << "[warning] No diffuse for tile with id " << i << '\n';
					auto layerData = AllocateTextureLayer();
					std::memset(layerData.get(), 0, BYTES_PER_LAYER);
					op.m_diffuseLayers.push_back(std::move(layerData));
				}
				else
				{
					op.m_diffuseLayers.emplace_back(op.LoadTextureLayer(parentPath / diffusePaths[i]));
				}
				
				TextureLayerPtr normalSpecData;
				
				if (!normalMapPaths[i].empty())
				{
					normalSpecData = op.LoadTextureLayer(parentPath / normalMapPaths[i]);
				}
				else
				{
					normalSpecData = AllocateTextureLayer();
					std::memset(normalSpecData.get(), 0x0000FFFF, BYTES_PER_LAYER);
				}
				
				if (!specularMapPaths[i].empty())
				{
					TextureLayerPtr specularData = op.LoadTextureLayer(parentPath / specularMapPaths[i]);
					for (size_t i = 0; i < BYTES_PER_LAYER; i += 4)
						normalSpecData.get()[i + 3] = specularData.get()[i];
				}
				
				op.m_normalSpecLayers.push_back(std::move(normalSpecData));
			}
			
			return op;
		});
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
	
	std::unique_ptr<TileGridMaterial> TileGridMaterialLoadOperation::FinishLoading()
	{
		std::unique_ptr<TileGridMaterial> material(new TileGridMaterial(m_layers));
		
		material->m_materialNames = std::move(m_materialNames);
		
		for (int i = 0; i < m_layers; i++)
		{
			glTextureSubImage3D(material->m_diffuseTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
			                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_diffuseLayers[i].get());
			
			glTextureSubImage3D(material->m_normalSpecTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
			                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_normalSpecLayers[i].get());
		}
		
		std::copy(m_materialSettings.begin(), m_materialSettings.end(), material->m_materialSettings);
		std::copy(m_isSolid.begin(), m_isSolid.end(), material->m_isSolid);
		
		material->m_diffuseTextureArray.SetupMipmapping(true);
		material->m_normalSpecTextureArray.SetupMipmapping(true);
		
		return material;
	}
}
