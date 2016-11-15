#pragma once

#include "memory/stackobject.h"
#include <type_traits>

namespace TankGame
{
	template <typename Tp, typename InitCallbackTp>
	class Lazy
	{
	public:
		explicit Lazy(InitCallbackTp initCallback)
		    : m_initCallback(initCallback) { }
		
		Tp& operator()()
		{
			if (m_value.IsNull())
				m_value.Construct(m_initCallback());
			return *m_value;
		}
		
	private:
		InitCallbackTp m_initCallback;
		
		StackObject<Tp> m_value;
	};
	
	template <typename InitCallbackTp>
	auto MakeLazy(InitCallbackTp init) -> Lazy<decltype(init()), InitCallbackTp>
	{
		return Lazy<decltype(init()), InitCallbackTp>(init);
	}
}
