#include "frames.h"

namespace TankGame
{
	static uint32_t frameQueueIndex = 0;
	
	uint32_t GetFrameQueueIndex()
	{
		return frameQueueIndex;
	}
	
	void AdvanceFrameQueueIndex()
	{
		frameQueueIndex = (frameQueueIndex + 1) % MAX_QUEUED_FRAMES;
	}
}
