#include "tilegridmaterial.h"
#include "gl/shadermodule.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <json.hpp>

#include "gl/specializationinfo.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<TileGridMaterial> TileGridMaterial::s_instance;
	
	constexpr GLsizei TileGridMaterial::WIDTH;
	constexpr GLsizei TileGridMaterial::HEIGHT;
	const int TileGridMaterial::MIPMAP_COUNT = Texture::GetMipmapCount(std::max(WIDTH, HEIGHT));
	
	static const std::string defaultMaterialName = "Unnamed material";
	
	static ShaderProgram LoadGridShader(GLsizei layerCount)
	{
		SpecializationInfo specInfo;
		specInfo.SetConstant("LAYER_COUNT", std::to_string(layerCount));
		
		fs::path shaderPath = GetResDirectory() / "shaders";
		auto vs = ShaderModule::FromFile(shaderPath / "tilegrid.vs.glsl", GL_VERTEX_SHADER, &specInfo);
		auto fs = ShaderModule::FromFile(shaderPath / "tilegrid.fs.glsl", GL_FRAGMENT_SHADER);
		
		return ShaderProgram({ &vs, &fs });
	}
	
	TileGridMaterial::TileGridMaterial(int layerCount)
	    : m_layerCount(layerCount),
	      m_diffuseTextureArray(WIDTH, HEIGHT, layerCount, MIPMAP_COUNT, GL_RGB8),
	      m_normalMapTextureArray(WIDTH, HEIGHT, layerCount, MIPMAP_COUNT, GL_RGB8),
	      m_specularMapTextureArray(WIDTH, HEIGHT, layerCount, MIPMAP_COUNT, GL_R8),
	      m_parametersBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(MaterialSettings) * layerCount, GL_MAP_WRITE_BIT)),
	      m_shader(LoadGridShader(layerCount))
	{
		std::fill(std::begin(m_isSolid), std::end(m_isSolid), false);
	}
	
	void TileGridMaterial::Bind() const
	{
		m_shader.Use();
		
		m_diffuseTextureArray.Bind(2);
		m_normalMapTextureArray.Bind(3);
		m_specularMapTextureArray.Bind(4);
		
		if (m_parametersNeedUpload)
		{
			void* memory = glMapNamedBufferRange(*m_parametersBuffer, 0, sizeof(MaterialSettings) * m_layerCount,
			                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			
			std::copy(m_materialSettings, m_materialSettings + m_layerCount, reinterpret_cast<MaterialSettings*>(memory));
			
			glUnmapNamedBuffer(*m_parametersBuffer);
			
			m_parametersNeedUpload = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, *m_parametersBuffer);
	}
	
	void TileGridMaterial::BindForShadowRender() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_isSolidBuffer->GetID());
	}
	
	const std::string& TileGridMaterial::GetMaterialName(uint8_t id) const
	{
		if (id >= m_materialNames.size())
			return defaultMaterialName;
		return m_materialNames[id];
	}
	
	TileGridMaterial TileGridMaterial::FromFile(const fs::path& path)
	{
		nlohmann::json jsonDocument = nlohmann::json::parse(ReadFileContents(path));
		
		int layers = 0;
		
		std::vector<std::string> diffusePaths;
		std::vector<std::string> normalMapPaths;
		std::vector<std::string> specularMapPaths;
		std::vector<std::string> materialNames;
		std::vector<MaterialSettings> materialSettings;
		std::vector<bool> isSolid;
		
		for (const nlohmann::json& tileElement : jsonDocument)
		{
			int id = tileElement["id"].get<int>();
			if (id < 0 || id > 255)
				throw std::runtime_error("Tile id out of range (" + std::to_string(id) + ").");
			
			if (id >= layers)
			{
				layers = id + 1;
				diffusePaths.resize(layers);
				normalMapPaths.resize(layers);
				specularMapPaths.resize(layers);
				materialNames.resize(layers);
				materialSettings.resize(layers);
				isSolid.resize(layers);
			}
			
			auto solidIt = tileElement.find("solid");
			if (solidIt == tileElement.end() || !solidIt->is_boolean())
				isSolid[id] = false;
			else
				isSolid[id] = solidIt->get<bool>();
			
			auto nameIt = tileElement.find("name");
			if (nameIt != tileElement.end())
				materialNames[id] = nameIt->get<std::string>();
			else
				materialNames[id] = defaultMaterialName;
			
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
				materialSettings[id].m_specularIntensity = (*specularIt)["intensity"].get<float>();
				materialSettings[id].m_specularExponent = (*specularIt)["exponent"].get<float>();
			}
			
			//Reads texture scale settings
			auto textureScaleIt = tileElement.find("textureScale");
			if (textureScaleIt == tileElement.end())
				materialSettings[id].m_textureScale = glm::vec2(1.0f);
			else
				materialSettings[id].m_textureScale = ParseVec2(*textureScaleIt);
		}
		
		TileGridMaterial material(layers);
		
		material.m_materialNames = std::move(materialNames);
		
		fs::path parentPath = path.parent_path();
		
		const uint8_t defaultNormal[] = { 0, 0, 255 };
		const uint8_t defaultSpecular[] = { 255 };
		
		std::copy(materialSettings.begin(), materialSettings.end(), material.m_materialSettings);
		std::copy(isSolid.begin(), isSolid.end(), material.m_isSolid);
		
		for (int i = 0; i < layers; i++)
		{
			if (diffusePaths[i].empty())
				GetLogStream() << "[warning] No diffuse for tile with id " << i << '\n';
			else
				material.m_diffuseTextureArray.LoadLayerFromFile(i, parentPath / diffusePaths[i]);
			
			if (normalMapPaths[i].empty())
				material.m_normalMapTextureArray.ClearLayer(i, GL_RGB, GL_UNSIGNED_BYTE, defaultNormal);
			else
				material.m_normalMapTextureArray.LoadLayerFromFile(i, parentPath / normalMapPaths[i]);
			
			if (specularMapPaths[i].empty())
				material.m_specularMapTextureArray.ClearLayer(i, GL_RED, GL_UNSIGNED_BYTE, defaultSpecular);
			else
				material.m_specularMapTextureArray.LoadLayerFromFile(i, parentPath / specularMapPaths[i]);
		}
		
		uint32_t isSolidPacked[256 / 32];
		std::fill(std::begin(isSolidPacked), std::end(isSolidPacked), 0);
		
		for (size_t i = 0; i < 256; i++)
		{
			if (i < isSolid.size() && isSolid[i])
				isSolidPacked[i / 32] |= 1 << (i % 32);
		}
		
		material.m_isSolidBuffer.Construct(sizeof(isSolidPacked), isSolidPacked, 0);
		
		material.m_diffuseTextureArray.SetupMipmapping(true);
		material.m_normalMapTextureArray.SetupMipmapping(true);
		material.m_specularMapTextureArray.SetupMipmapping(true);
		
		return material;
	}
}
