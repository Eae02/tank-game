#include "profiling.h"
#include "platform/common.h"

namespace TankGame
{
	bool enableCPUTimers;
	
	static int currentDepth = 0;
	std::vector<CPUTimer> cpuTimers;
	
	ScopedCPUTimer::ScopedCPUTimer(const char* name1, const char* name2)
	{
		if (enableCPUTimers)
		{
			std::string fullName = name1;
			if (name2)
			{
				fullName += " - ";
				fullName += name2;
			}
			
			startTime = GetTime();
			index = cpuTimers.size();
			cpuTimers.emplace_back(CPUTimer { std::move(fullName), 0.0, currentDepth });
			currentDepth++;
		}
	}
	
	ScopedCPUTimer::~ScopedCPUTimer()
	{
		if (enableCPUTimers)
		{
			cpuTimers[index].elapsedTime = GetTime() - startTime;
			currentDepth--;
		}
	}
}
