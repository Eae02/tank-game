#pragma once

#include <cstddef>
#include <cstdint>

namespace TankGame
{
	enum class Key
	{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		D1, D2,
		F1, F2, F3, F4, F5,
		ArrowUp,
		ArrowDown,
		ArrowLeft,
		ArrowRight,
		End,
		Home,
		Enter,
		Space,
		Tab,
		Backspace,
		Delete,
		Escape,
		PageUp,
		PageDown,
		GraveAccent,
		LShift,
		RShift,
		LControl,
		RControl,
		LAlt,
		RAlt,
		LSuper,
		RSuper
	};
	
	constexpr int MAX_KEY_ID = (int)Key::RSuper;
	static_assert(MAX_KEY_ID <= 63);
	
	inline constexpr uint64_t KeyToBitmask(Key key) { return (uint64_t)1 << (uint64_t)key; }
	
	constexpr uint64_t KEY_MASK_SHIFT   = KeyToBitmask(Key::LShift) | KeyToBitmask(Key::RShift);
	constexpr uint64_t KEY_MASK_CONTROL = KeyToBitmask(Key::LControl) | KeyToBitmask(Key::RControl);
	constexpr uint64_t KEY_MASK_ALT     = KeyToBitmask(Key::LAlt) | KeyToBitmask(Key::RAlt);
	constexpr uint64_t KEY_MASK_SUPER   = KeyToBitmask(Key::LSuper) | KeyToBitmask(Key::RSuper);
	
	class Keyboard
	{
	public:
		friend class Window;
		
		Keyboard() = default;
		
		bool IsDown(Key key, bool ignoreCapture = false) const
		{ return IsAnyDown(KeyToBitmask(key), ignoreCapture); }
		bool WasDown(Key key, bool ignoreCapture = false) const
		{ return WasAnyDown(KeyToBitmask(key), ignoreCapture); }
		
		bool IsAnyDown(uint64_t bitmask, bool ignoreCapture = false) const
		{
			if (isCaptured && !ignoreCapture) return false;
			return m_keyStateBitmask & bitmask;
		}
		
		bool WasAnyDown(uint64_t bitmask, bool ignoreCapture = false) const
		{
			if (m_wasCaptured && !ignoreCapture) return false;
			return m_preKeyStateBitmask & bitmask;
		}
		
		bool isCaptured = false;
		
	private:
		void OnFrameEnd() //Called by Window
		{
			m_preKeyStateBitmask = m_keyStateBitmask;
			m_wasCaptured = isCaptured;
		}
		
		uint64_t m_keyStateBitmask = 0;
		uint64_t m_preKeyStateBitmask = 0;
		
		bool m_wasCaptured = false;
	};
}
