#include "tilegridmaterialloadoperation.h"
#include "textureloadoperation.h"
#include "../utils/jsonparseutils.h"
#include "../utils/ioutils.h"

#include <json.hpp>
#include <stb_image.h>
#include <stb_image_resize.h>

namespace TankGame
{
	TileGridMaterialLoadOperation::TileGridMaterialLoadOperation(fs::path jsonPath,
	      TileGridMaterialLoadOperation::DoneCallback doneCallback)
	    : m_doneCallback(doneCallback), m_jsonPath(std::move(jsonPath))
	{
		
	}
	
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
				m_diffuseLayers.emplace_back(CreateLayerPtr());
			}
			else
			{
				m_diffuseLayers.emplace_back(LoadTextureLayer(parentPath / diffusePaths[i]));
			}
			
			if (!normalMapPaths[i].empty())
				m_normalMapLayers.emplace_back(LoadTextureLayer(parentPath / normalMapPaths[i]));
			else
				m_normalMapLayers.emplace_back(CreateLayerPtr());
			
			if (!specularMapPaths[i].empty())
				m_specularMapLayers.emplace_back(LoadTextureLayer(parentPath / specularMapPaths[i]));
			else
				m_specularMapLayers.emplace_back(CreateLayerPtr());
		}
	}
	
	void TileGridMaterialLoadOperation::ProcessResult()
	{
		TileGridMaterial material(m_layers);
		
		material.m_materialNames = std::move(m_materialNames);
		
		const uint8_t defaultNormal[] = { 0, 0, 255 };
		const uint8_t defaultSpecular[] = { 255 };
		
		for (int i = 0; i < m_layers; i++)
		{
			if (m_diffuseLayers[i] != nullptr)
			{
				glTextureSubImage3D(material.m_diffuseTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
				                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_diffuseLayers[i].get());
			}
			
			if (m_normalMapLayers[i] != nullptr)
			{
				glTextureSubImage3D(material.m_normalMapTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
				                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_normalMapLayers[i].get());
			}
			else
			{
				glClearTexSubImage(material.m_normalMapTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
				                   TileGridMaterial::HEIGHT, 1, GL_RGB, GL_UNSIGNED_BYTE, defaultNormal);
			}
			
			if (m_specularMapLayers[i] != nullptr)
			{
				glTextureSubImage3D(material.m_specularMapTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
				                    TileGridMaterial::HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_specularMapLayers[i].get());
			}
			else
			{
				glClearTexSubImage(material.m_specularMapTextureArray.GetID(), 0, 0, 0, i, TileGridMaterial::WIDTH,
				                   TileGridMaterial::HEIGHT, 1, GL_RED, GL_UNSIGNED_BYTE, defaultSpecular);
			}
		}
		
		std::copy(m_materialSettings.begin(), m_materialSettings.end(), material.m_materialSettings);
		std::copy(m_isSolid.begin(), m_isSolid.end(), material.m_isSolid);
		
		uint32_t isSolidPacked[256 / 32];
		std::fill(std::begin(isSolidPacked), std::end(isSolidPacked), 0);
		
		for (size_t i = 0; i < 256; i++)
		{
			if (i < m_isSolid.size() && m_isSolid[i])
				isSolidPacked[i / 32] |= 1 << (i % 32);
		}
		
		material.m_isSolidBuffer.Construct(sizeof(isSolidPacked), isSolidPacked, 0);
		
		material.m_diffuseTextureArray.SetupMipmapping(true);
		material.m_normalMapTextureArray.SetupMipmapping(true);
		material.m_specularMapTextureArray.SetupMipmapping(true);
		
		m_doneCallback(std::move(material));
	}
	
	TileGridMaterialLoadOperation::TextureLayerPtr TileGridMaterialLoadOperation::CreateLayerPtr()
	{
		return { nullptr, &TextureLoadOperation::STBIDataDeleter };
	}
	
	TileGridMaterialLoadOperation::TextureLayerPtr TileGridMaterialLoadOperation::LoadTextureLayer(const fs::path& path)
	{
		int width, height;
		
		TextureLayerPtr layerData = CreateLayerPtr();
		layerData.reset(stbi_load(path.c_str(), &width, &height, nullptr, 4));
		
		if (width == TileGridMaterial::WIDTH && height == TileGridMaterial::HEIGHT)
			return layerData;
		
		TextureLayerPtr resizedData = CreateLayerPtr();
		resizedData.reset(reinterpret_cast<uint8_t*>(malloc(TileGridMaterial::WIDTH * TileGridMaterial::HEIGHT * 4)));
		
		stbir_resize_uint8(layerData.get(), width, height, 0, resizedData.get(),
		                   TileGridMaterial::WIDTH, TileGridMaterial::HEIGHT, 0, 4);
		
		return resizedData;
	}
}
