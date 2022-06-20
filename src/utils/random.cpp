#include "random.h"

#include <chrono>
using namespace std::chrono;

namespace TankGame
{
	pcg64_fast globalRNG(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}
