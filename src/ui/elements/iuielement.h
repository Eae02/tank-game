#pragma once

#include "../../utils/abstract.h"


namespace TankGame
{
	class IUIElement : public Abstract
	{
	public:
		virtual void SetPosition(glm::vec2 position) = 0;
		virtual void Draw(const class UIRenderer& uiRenderer) const = 0;
		
		virtual glm::vec2 GetSize() const = 0;
		
		static void PlayMouseOverEffect();
	};
}
