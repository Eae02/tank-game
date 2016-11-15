#pragma once

#include "../../utils/abstract.h"

#include <memory>
#include <json.hpp>

namespace TankGame
{
	template <typename T>
	class IClassParser : public Abstract
	{
	public:
		virtual std::unique_ptr<T> Parse(const nlohmann::json& json) const = 0;
	};
}
