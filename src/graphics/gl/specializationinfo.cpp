#include "specializationinfo.h"

#include <sstream>

namespace TankGame
{
	std::string SpecializationInfo::GetSourceString() const
	{
		std::ostringstream stream;
		for (const SpecializationConstant& constant : m_constants)
			stream << "#define " << constant.m_name << " " << constant.m_value << "\n";
		return stream.str();
	}
}
