#pragma once


namespace TankGame
{
	constexpr uint32_t MAX_QUEUED_FRAMES = 3;
	
	uint32_t GetFrameQueueIndex();
	void AdvanceFrameQueueIndex();
}
