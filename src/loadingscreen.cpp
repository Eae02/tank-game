#include "loadingscreen.h"
#include "updateinfo.h"
#include "tanktextures.h"
#include "world/props/propclassloadoperation.h"
#include "world/props/propsmanager.h"
#include "world/entities/conveyorbeltentity.h"
#include "graphics/ui/uirenderer.h"
#include "graphics/tilegridmaterial.h"
#include "graphics/tilegridmaterialloadoperation.h"
#include "utils/ioutils.h"
#include "audio/soundsmanager.h"

#include <cmath>

namespace TankGame
{
	LoadingScreen::LoadingScreen()
	    : m_loadingSprite(Texture2D::FromFile(resDirectoryPath / "ui" / "loading.png"))
	{
		
	}
	
	void LoadingScreen::Initialize()
	{
		m_workList.Add(std::async([]
		{
			return std::make_unique<SoundsManager>(resDirectoryPath / "audio" / "audio.json");
		}), SoundsManager::SetInstance);
		
		m_workList.Add(PropClassLoadOperation::Load(resDirectoryPath / "props"),
			[] (PropClassLoadOperation op) { PropsManager::SetInstance(op.FinishLoading()); });
		
		TankTextures::LoadAndCreateInstance(m_workList);
		
		m_workList.Add(
			TileGridMaterialLoadOperation::Load(resDirectoryPath / "tiles" / "tiles.json"),
			[] (TileGridMaterialLoadOperation op) { TileGridMaterial::SetInstance(op.FinishLoading()); });
		
		ConveyorBeltEntity::LoadResources(m_workList);
	}
	
	void LoadingScreen::RunFrame()
	{
		m_workList.Poll();
		
		glm::vec4 clearColor(ParseColorHexCodeSRGB(0x224E57), 1.0f);
		glClearBufferfv(GL_COLOR, 0, reinterpret_cast<const GLfloat*>(&clearColor));
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		UIRenderer::GetInstance().DrawSprite(m_loadingSprite, m_loadingSpriteRect, glm::vec4(1.0f));
		
		glDisable(GL_BLEND);
	}
	
	bool LoadingScreen::IsLoadingDone()
	{
		return m_workList.IsDone();
	}
	
	void LoadingScreen::SetWindowSize(int width, int height)
	{
		m_loadingSpriteRect = Rectangle::CreateCentered(glm::vec2(width, height) / 2.0f, m_loadingSprite.GetWidth(),
		                                                m_loadingSprite.GetHeight());
	}
}
