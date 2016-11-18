#include "tanktextures.h"
#include "exceptions/invalidstateexception.h"
#include "utils/utils.h"
#include "utils/ioutils.h"
#include "graphics/textureloadoperation.h"

namespace TankGame
{
	static std::unique_ptr<TankTextures> instance;
	
	class TankTexturesLoadOperation : public IASyncOperation
	{
	public:
		TankTexturesLoadOperation()
		    : m_baseDiffuseLoadOps{ TextureLoadOperation(GetResDirectory() / "tank" / "base0.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base1.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base2.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base3.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base4.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base5.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base6.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base7.png", nullptr),
		                            TextureLoadOperation(GetResDirectory() / "tank" / "base8.png", nullptr) },
		      m_baseNormalsLoadOp(GetResDirectory() / "tank" / "base-normals.png", nullptr) { }
		
		virtual void DoWork() override
		{
			for (TextureLoadOperation& loadOperation : m_baseDiffuseLoadOps)
				loadOperation.DoWork();
			m_baseNormalsLoadOp.DoWork();
		}
		
		virtual void ProcessResult() override
		{
			instance = std::make_unique<TankTextures>(m_baseDiffuseLoadOps[0].CreateTexture(),
			        m_baseDiffuseLoadOps[1].CreateTexture(), m_baseDiffuseLoadOps[2].CreateTexture(),
			        m_baseDiffuseLoadOps[3].CreateTexture(), m_baseDiffuseLoadOps[4].CreateTexture(),
			        m_baseDiffuseLoadOps[5].CreateTexture(), m_baseDiffuseLoadOps[6].CreateTexture(),
			        m_baseDiffuseLoadOps[7].CreateTexture(), m_baseDiffuseLoadOps[8].CreateTexture(),
			        m_baseNormalsLoadOp.CreateTexture());
		}
		
	private:
		TextureLoadOperation m_baseDiffuseLoadOps[9];
		TextureLoadOperation m_baseNormalsLoadOp;
	};
	
	TankTextures::TankTextures(Texture2D&& baseDiffuse0, Texture2D&& baseDiffuse1, Texture2D&& baseDiffuse2,
	                           Texture2D&& baseDiffuse3, Texture2D&& baseDiffuse4, Texture2D&& baseDiffuse5,
	                           Texture2D&& baseDiffuse6, Texture2D&& baseDiffuse7, Texture2D&& baseDiffuse8,
	                           Texture2D&& baseNormals)
	    : m_baseDiffuse{ std::move(baseDiffuse0), std::move(baseDiffuse1), std::move(baseDiffuse2),
	                     std::move(baseDiffuse3), std::move(baseDiffuse4), std::move(baseDiffuse5),
	                     std::move(baseDiffuse6), std::move(baseDiffuse7), std::move(baseDiffuse8) },
	      m_baseNormals(std::move(baseNormals)),
	      m_baseMaterials{ SpriteMaterial(m_baseDiffuse[0], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[1], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[2], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[3], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[4], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[5], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[6], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[7], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[8], m_baseNormals, 1, 30)
	      }
	{
		for (Texture2D& texture : m_baseDiffuse)
			texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		m_baseNormals.SetWrapMode(GL_CLAMP_TO_EDGE);
	}
	
	std::unique_ptr<IASyncOperation> TankTextures::CreateInstance()
	{
		if (instance != nullptr)
			throw InvalidStateException("Tank textures already loaded.");
		CallOnClose([] { instance = nullptr; });
		
		return std::make_unique<TankTexturesLoadOperation>();
	}
	
	const TankTextures& TankTextures::GetInstance()
	{ return *instance; }
}
