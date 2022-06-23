#pragma once

#include <glm/glm.hpp>

#include "../world/path/path.h"

namespace TankGame
{
	class EnemyAI
	{
	public:
		struct AIParameters
		{
			float m_movementSpeed;
			float m_circleRadius;
			
			inline AIParameters(float movementSpeed, float circleRadius)
				: m_movementSpeed(movementSpeed), m_circleRadius(circleRadius) { }
		};
		
		enum class States
		{
			Idle,
			Searching,
			ReturningToIdle,
			Chasing
		};
		
		EnemyAI(AIParameters parameters, Path idlePath);
		
		void Update(class EnemyTank& entity, glm::vec2 playerPosition, const class UpdateInfo& updateInfo);
		
		void DetectPlayer(class EnemyTank& entity, glm::vec2 playerPosition);
		
		inline void SetGameWorld(const class GameWorld* gameWorld)
		{ m_gameWorld = gameWorld; }
		
		inline const Path& GetIdlePath() const
		{ return m_idlePath; }
		inline Path& GetIdlePath()
		{ return m_idlePath; }
		
		inline void SetIdlePathProgress(float progress)
		{ m_idlePathProgress = progress; }
		
		void SetIdlePath(Path idlePath);
		
		void IdlePathChanged();
		
	private:
		bool MaybeBeginChasing(glm::vec2 selfPosition, glm::vec2 playerPosition);
		
		const class GameWorld* m_gameWorld;
		
		AIParameters m_parameters;
		Path m_idlePath;
		
		//This is either the path to the entity (if chasing) or the path back to the idle path (if returning to idle).
		Path m_path;
		
		float m_chasePathProgress;
		
		static constexpr double PATH_UPDATE_INTERVAL = 0.5;
		double m_lastPathUpdateTime;
		
		float m_returnToIdleProgress;
		
		float m_idlePathProgress = 0.0f;
		
		States m_state = States::Idle;
	};
}
