#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace TankGame
{
	extern bool enableCPUTimers;
	
	struct CPUTimer
	{
		std::string name;
		double elapsedTime;
		int depth;
	};
	extern std::vector<CPUTimer> cpuTimers;
	
	struct ScopedCPUTimer
	{
		ScopedCPUTimer(const char* name1, const char* name2);
		~ScopedCPUTimer();
		double startTime;
		size_t index;
	};
}

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define FUNC_TIMER ScopedCPUTimer COMBINE(_timer, __LINE__) (__PRETTY_FUNCTION__, nullptr);
#define SCOPE_TIMER(name) ScopedCPUTimer COMBINE(_timer, __LINE__)(__PRETTY_FUNCTION__, name);
