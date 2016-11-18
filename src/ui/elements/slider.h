#pragma once

#include "iuielement.h"
#include "../../graphics/gl/texture2d.h"
#include "../../utils/memory/stackobject.h"
#include "../../rectangle.h"

namespace TankGame
{
	class Slider : public IUIElement
	{
	public:
		Slider(float min, float max, float snap = 0.0f);
		
		bool Update(const class UpdateInfo& updateInfo, float& value);
		
		virtual void SetPosition(glm::vec2 position) override;
		virtual glm::vec2 GetSize() const override;
		virtual void Draw(const class UIRenderer& uiRenderer) const override;
		
	private:
		static StackObject<Texture2D> s_knobTexture;
		static StackObject<Texture2D> s_labelTexture;
		
		bool m_isMoving = false;
		float m_labelOpacity = 0.0f;
		
		Rectangle m_rectangle;
		
		float m_min, m_max, m_snap;
		float m_position = 0;
	};
}
