#pragma once

#include <string>
#include <iosfwd>
#include <vector>
#include <glm/glm.hpp>

#include "graphics/gl/texture2d.h"

namespace TankGame
{
	class LevelMenuInfo
	{
	public:
		LevelMenuInfo() = default;
		
		[[nodiscard]] bool Load(std::istream& stream);
		
#ifndef __EMSCRIPTEN__
		static void WriteMenuInfo(const class Level& level, const std::string& name, std::ostream& outputStream);
#endif
		
		inline const std::string& GetName() const
		{ return m_name; }
		inline void SetName(std::string name)
		{ m_name = std::move(name); }
		
		inline size_t GetCheckpointCount() const
		{ return m_checkpoints.size(); }
		
		inline const Texture2D& GetCheckpointImage(size_t index) const
		{ return m_checkpointImages[m_checkpoints[index].m_imageIndex]; }
		inline int GetCheckpointIndex(size_t index) const
		{ return m_checkpoints[index].m_checkpointIndex; }
		
		inline const Texture2D& GetStartImage() const
		{ return m_checkpointImages[0]; }
		
	private:
		struct Checkpoint
		{
			int m_checkpointIndex;
			size_t m_imageIndex;
			glm::vec2 m_position;
		};
		
		std::string m_name;
		
		std::vector<Checkpoint> m_checkpoints;
		std::vector<Texture2D> m_checkpointImages;
	};
}
