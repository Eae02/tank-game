#include "loadingscreen.h"
#include "updateinfo.h"
#include "tanktextures.h"
#include "world/props/propsmanager.h"
#include "world/entities/conveyorbeltentity.h"
#include "graphics/ui/uirenderer.h"
#include "graphics/tilegridmaterial.h"
#include "graphics/tilegridmaterialloadoperation.h"
#include "utils/ioutils.h"
#include "audio/soundsmanager.h"
#include "audio/soundloadoperation.h"

#include <cmath>
#include <GLFW/glfw3.h>

namespace TankGame
{
	LoadingScreen::LoadingScreen()
	    : m_loadingSprite(Texture2D::FromFile(GetResDirectory() / "ui" / "loading.png"))
	{
		
	}
	
	void LoadingScreen::Initialize()
	{
		m_workList.SubmitWork(std::make_unique<SoundLoadOperation>(GetResDirectory() / "audio" / "audio.json",
		                                                           [] (SoundsManager&& soundsManager)
		{
			SoundsManager::SetInstance(std::make_unique<SoundsManager>(std::move(soundsManager)));
		}));
		
		m_workList.SubmitWork(PropsManager::GetInstance().LoadPropClasses(GetResDirectory() / "props"));
		
		m_workList.SubmitWork(TankTextures::CreateInstance());
		
		m_workList.SubmitWork(std::make_unique<TileGridMaterialLoadOperation>(GetResDirectory() / "tiles" / "tiles.json",
		                                                                      [] (TileGridMaterial&& material)
		{
			TileGridMaterial::SetInstance(std::make_unique<TileGridMaterial>(std::move(material)));
		}));
		
		ConveyorBeltEntity::LoadResources(m_workList);
		
		m_workList.BeginProcessing();
	}
	
	void LoadingScreen::RunFrame()
	{
		m_workList.ProcessSingleResult();
		
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
