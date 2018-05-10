#pragma once

#include "ipropertiesobject.h"

#include <string>

namespace TankGame
{
	class PropertiesWindow
	{
	public:
		explicit PropertiesWindow(IPropertiesObject* object = nullptr);
		
		void SetObject(IPropertiesObject* object);
		inline IPropertiesObject* GetObject() const
		{ return m_object; }
		
		void Render();
		
		inline bool ShouldClose() const
		{ return !m_isOpen; }
		
		inline bool HasFocus() const
		{ return m_hasFocus; }
		
	private:
		static uint64_t s_nextWindowID;
		
		IPropertiesObject* m_object;
		
		uint64_t m_windowID;
		std::string m_label;
		
		bool m_isOpen = true;
		bool m_resetSize = true;
		bool m_setPositionToCursor = true;
		bool m_hasFocus = false;
	};
}
