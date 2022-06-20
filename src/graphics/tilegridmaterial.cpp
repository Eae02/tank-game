#include "tilegridmaterial.h"
#include "gl/shadermodule.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

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
		
		fs::path shaderPath = resDirectoryPath / "shaders";
		auto vs = ShaderModule::FromFile(shaderPath / "tilegrid.vs.glsl", GL_VERTEX_SHADER, &specInfo);
		auto fs = ShaderModule::FromFile(shaderPath / "tilegrid.fs.glsl", GL_FRAGMENT_SHADER);
		
		ShaderProgram program({ &vs, &fs });
		program.SetUniformBlockBinding("MaterialSettingsUB", TileGridMaterial::MATERIAL_SETTINGS_BUFFER_BINDING);
		program.SetTextureBinding("tileIDSampler", 0);
		program.SetTextureBinding("tileRotationSampler", 1);
		program.SetTextureBinding("diffuseMaps", 2);
		program.SetTextureBinding("normalSpecMaps", 3);
		return program;
	}
	
	TileGridMaterial::TileGridMaterial(int layerCount)
	    : m_layerCount(layerCount),
	      m_diffuseTextureArray(WIDTH, HEIGHT, layerCount, MIPMAP_COUNT, GL_RGB8),
	      m_normalSpecTextureArray(WIDTH, HEIGHT, layerCount, MIPMAP_COUNT, GL_RGBA8),
	      m_parametersBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(MaterialSettings) * layerCount, BufferUsage::DynamicData)),
	      m_shader(LoadGridShader(layerCount))
	{
		std::fill(std::begin(m_isSolid), std::end(m_isSolid), false);
		
		m_renderAreaOffsetUniformLoc = m_shader.GetUniformLocation("renderAreaOffset");
		m_renderAreaWidthUniformLoc = m_shader.GetUniformLocation("renderAreaWidth");
	}
	
	void TileGridMaterial::Bind(glm::ivec2 minVisibleTile, glm::ivec2 maxVisibleTile) const
	{
		m_shader.Use();
		
		glUniform2i(m_renderAreaOffsetUniformLoc, minVisibleTile.x, minVisibleTile.y);
		glUniform1i(m_renderAreaWidthUniformLoc, maxVisibleTile.x - minVisibleTile.x);
		
		m_diffuseTextureArray.Bind(2);
		m_normalSpecTextureArray.Bind(3);
		
		if (m_parametersNeedUpload)
		{
			m_parametersBuffer->Update(0, sizeof(MaterialSettings) * m_layerCount, m_materialSettings);
			m_parametersNeedUpload = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_SETTINGS_BUFFER_BINDING, m_parametersBuffer->GetID());
	}
	
	const std::string& TileGridMaterial::GetMaterialName(uint8_t id) const
	{
		if (id >= m_materialNames.size())
			return defaultMaterialName;
		return m_materialNames[id];
	}
}
