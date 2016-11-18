#pragma once

#include "asyncworklist.h"
#include "rectangle.h"
#include "graphics/gl/texture2d.h"

namespace TankGame
{
	class LoadingScreen
	{
	public:
		LoadingScreen();
		
		void Initialize();
		
		void RunFrame();
		
		bool IsLoadingDone();
		
		void SetWindowSize(int width, int height);
		
	private:
		Texture2D m_loadingSprite;
		Rectangle m_loadingSpriteRect;
		
		ASyncWorkList m_workList;
	};
}
