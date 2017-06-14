#pragma once

#include "registryreference.h"
#include "luainc.h"

#include <list>

namespace TankGame
{
namespace Lua
{
	class Sandbox
	{
	public:
		Sandbox(lua_State* state);
		
		Sandbox(Sandbox&& other) = delete;
		Sandbox& operator=(Sandbox&& other) = delete;
		
		inline void PushTable(lua_State* state) const
		{ m_sandboxTableRef.Load(state); }
		
		void Update(float dt);
		
	private:
		RegistryReference m_sandboxTableRef;
		
		struct Timer
		{
			float m_timeRemaining;
			RegistryReference m_callback;
		};
		
		std::list<Timer> m_timers;
		
		struct UpdateCallback
		{
			uint64_t m_id;
			RegistryReference m_callback;
		};
		
		std::list<UpdateCallback> m_updateCallbacks;
		
		uint64_t m_nextUpdateCallbackID = 0;
	};
}
}
