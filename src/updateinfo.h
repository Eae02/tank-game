#pragma once

namespace TankGame
{
	class UpdateInfo
	{
	public:
		inline UpdateInfo(float dt, float gameTime, const class Keyboard& keyboard, const class Mouse& mouse,
		                  const class ViewInfo& viewInfo, int windowWidth, int windowHeight,
						  bool isEditorOpen = false, bool windowHasFocus = true)
		    : m_dt(dt), m_gameTime(gameTime), m_keyboard(keyboard), m_mouse(mouse), m_viewInfo(viewInfo),
		      m_windowWidth(windowWidth), m_windowHeight(windowHeight), m_isEditorOpen(isEditorOpen),
			  m_windowHasFocus(windowHasFocus) { }
		
		float m_dt;
		float m_gameTime;
		
		const struct VideoModes* m_videoModes = nullptr;
		const class Keyboard& m_keyboard;
		const class Mouse& m_mouse;
		const class ViewInfo& m_viewInfo;
		
		int m_windowWidth;
		int m_windowHeight;
		bool m_isEditorOpen;
		bool m_windowHasFocus;
	};
}
