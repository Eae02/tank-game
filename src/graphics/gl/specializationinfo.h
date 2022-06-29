#pragma once


namespace TankGame
{
	class SpecializationInfo
	{
	public:
		void SetConstant(const std::string& name, const std::string& value)
		{
			m_constants.push_back({ name, value });
		}
		
		std::string GetSourceString() const;
		
	private:
		struct SpecializationConstant
		{
			std::string m_name;
			std::string m_value;
		};
		
		std::vector<SpecializationConstant> m_constants;
	};
}
