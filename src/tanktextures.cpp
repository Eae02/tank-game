#include "tanktextures.h"
#include "exceptions/invalidstateexception.h"
#include "utils/utils.h"
#include "utils/ioutils.h"
#include "graphics/textureloadoperation.h"
#include "asyncworklist.h"

namespace TankGame
{
	std::unique_ptr<TankTextures> TankTextures::s_instance;
	
	static constexpr float TANK_SPEC_INTENSITY = 1;
	static constexpr float TANK_SPEC_EXPONENT = 30;
	
	TankTextures::TankTextures(std::vector<Texture2D> baseDiffuse, Texture2D baseNormals)
		: m_baseDiffuse(std::move(baseDiffuse)), m_baseNormals(std::move(baseNormals))
	{
		m_baseMaterials.reserve(m_baseDiffuse.size());
		for (size_t i = 0; i < m_baseDiffuse.size(); i++)
		{
			m_baseMaterials.emplace_back(m_baseDiffuse[i], m_baseNormals, TANK_SPEC_INTENSITY, TANK_SPEC_EXPONENT);
		}
	}
	
	struct TankTextureLoadOperations
	{
		std::array<TextureLoadOperation, 9> baseDiffuse;
		TextureLoadOperation baseNormals;
	};
	
	void TankTextures::LoadAndCreateInstance(ASyncWorkList& asyncWorkList)
	{
		asyncWorkList.Add(std::async([]
		{
			TankTextureLoadOperations ops;
			for (int i = 0; i < 9; i++)
			{
				char name[] = "base0.png";
				name[4] += i;
				ops.baseDiffuse[i] = TextureLoadOperation::Load(resDirectoryPath / "tank" / name);
			}
			ops.baseNormals = TextureLoadOperation::Load(resDirectoryPath / "tank" / "base-normals.png");
			return ops;
		}), [] (TankTextureLoadOperations ops)
		{
			std::vector<Texture2D> baseDiffuseTextures;
			baseDiffuseTextures.reserve(ops.baseDiffuse.size());
			for (size_t i = 0; i < ops.baseDiffuse.size(); i++)
				baseDiffuseTextures.push_back(ops.baseDiffuse[i].CreateTexture());
			
			s_instance.reset(new TankTextures(std::move(baseDiffuseTextures), ops.baseNormals.CreateTexture()));
			
			CallOnClose([] { s_instance = nullptr; });
		});
	}
}
