#include "iuielement.h"
#include "../../audio/soundeffectplayer.h"

namespace TankGame
{
	static SoundEffectPlayer mouseOverPlayer("MenuMouseOver");
	
	void IUIElement::PlayMouseOverEffect()
	{
		mouseOverPlayer.Play(glm::vec2(), 0.5f, 1.0f);
	}
}
