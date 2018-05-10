#include "enemyai.h"
#include "aiutils.h"
#include "../world/entities/enemies/enemytank.h"
#include "../world/gameworld.h"
#include "../world/path/pathfinder.h"
#include "../world/spteams.h"
#include "../utils/mathutils.h"
#include "../utils/utils.h"
#include "../updateinfo.h"

#include <glm/gtc/constants.hpp>
#include <iostream>
#include <GLFW/glfw3.h>

namespace TankGame
{
	bool EnemyAI::MaybeBeginChasing(glm::vec2 playerPosition)
	{
		Path newPath;
		
		if (FindPath(*m_gameWorld, m_entity.GetTransform().GetPosition(),
		             playerPosition, newPath, m_parameters.m_circleRadius))
		{
			m_chasePathProgress = 0.0f;
			m_lastPathUpdateTime = glfwGetTime();
			m_state = States::Chasing;
			m_path = std::move(newPath);
			return true;
		}
		
		return false;
	}
	
	EnemyAI::EnemyAI(EnemyTank& entity, AIParameters parameters, Path idlePath)
	    : m_entity(entity), m_parameters(std::move(parameters)), m_idlePath(std::move(idlePath)) { }
	
	void EnemyAI::Update(glm::vec2 playerPosition, const UpdateInfo& updateInfo)
	{
		glm::vec2 toPlayer = playerPosition - m_entity.GetTransform().GetPosition();
		const float SIGHT_RADIUS = 8;
		const float FIRE_RADIUS = 7;
		
		const bool isPlayerVisible = IsPointVisible(playerPosition) && LengthSquared(toPlayer) < SIGHT_RADIUS * SIGHT_RADIUS;
		
		switch (m_state)
		{
			case States::Idle:
			{
				if (isPlayerVisible)
					MaybeBeginChasing(playerPosition);
				else
				{
					WalkPath(updateInfo.m_dt, m_idlePath, m_idlePathProgress, true);
					m_entity.SetCannonRotation(m_entity.GetTransform().GetRotation());
				}
				
				break;
			}
			
			case States::Chasing:
			{
				if (!isPlayerVisible)
				{
					m_state = States::Searching;
					break;
				}
				
				m_entity.SetCannonRotation(glm::half_pi<float>() + std::atan2(toPlayer.y, toPlayer.x));
				
				const bool withinFiringRange = LengthSquared(toPlayer) < FIRE_RADIUS * FIRE_RADIUS;
				
				if (m_entity.CanFire(updateInfo.m_gameTime) && withinFiringRange)
				{
					if (m_entity.IsRocketTank())
						m_entity.FireRocket(35, updateInfo.m_gameTime, { });
					else
						m_entity.FirePlasmaGun(ParseColorHexCodeSRGB(0xFF564A), 5, updateInfo.m_gameTime, { });
				}
				
				double time = glfwGetTime();
				if (time > m_lastPathUpdateTime + PATH_UPDATE_INTERVAL)
				{
					m_chasePathProgress = 0.0f;
					m_lastPathUpdateTime = time;
					
					Path newPath;
					
					if (FindPath(*m_gameWorld, m_entity.GetTransform().GetPosition(),
					             playerPosition, newPath, m_parameters.m_circleRadius))
					{
						m_path = std::move(newPath);
					}
					else
					{
						m_state = States::Searching;
						break;
					}
				}
				
				WalkPath(updateInfo.m_dt, m_path, m_chasePathProgress);
				break;
			}
			
			case States::Searching:
			{
				WalkPath(updateInfo.m_dt, m_path, m_chasePathProgress);
				
				if (isPlayerVisible && MaybeBeginChasing(playerPosition))
					break;
				
				m_entity.SetCannonRotation(m_entity.GetTransform().GetRotation());
				
				if (m_chasePathProgress >= m_path.GetTotalLength())
				{
					Path newPath;
					
					auto returnPoint = m_idlePath.GetClosestPointOnPath(m_entity.GetTransform().GetPosition());
					
					if (!FindPath(*m_gameWorld, m_entity.GetTransform().GetPosition(), returnPoint.m_position, newPath,
					              m_parameters.m_circleRadius))
					{
						//This shouldn't happen, but if it does the entity is moved back to the start of the idle path.
						GetLogStream() << "[error] AI panic: No return path found\n";
						m_idlePathProgress = 0.0f;
						m_state = States::Idle;
					}
					else
					{
						m_idlePathProgress = returnPoint.m_progress;
						m_returnToIdleProgress = 0.0f;
						m_path = std::move(newPath);
						m_state = States::ReturningToIdle;
					}
				}
				
				break;
			}
			
			case States::ReturningToIdle:
			{
				if (isPlayerVisible && MaybeBeginChasing(playerPosition))
					break;
				
				WalkPath(updateInfo.m_dt, m_path, m_returnToIdleProgress);
				m_entity.SetCannonRotation(m_entity.GetTransform().GetRotation());
				
				if (m_returnToIdleProgress >= m_path.GetTotalLength())
					m_state = States::Idle;
				
				break;
			}
		}
	}
	
	void EnemyAI::DetectPlayer(glm::vec2 playerPosition)
	{
		if (m_state == States::Idle || m_state == States::ReturningToIdle)
		{
			Path path;
			
			if (FindPath(*m_gameWorld, m_entity.GetTransform().GetPosition(), playerPosition, path,
			             m_parameters.m_circleRadius))
			{
				m_path = std::move(path);
				m_chasePathProgress = 0;
				m_state = States::Searching;
			}
		}
	}
	
	void EnemyAI::SetIdlePath(Path idlePath)
	{
		m_idlePath = std::move(idlePath);
		IdlePathChanged();
	}
	
	void EnemyAI::IdlePathChanged()
	{
		if (m_state == States::Idle || m_state == States::ReturningToIdle)
		{
			m_state = States::Idle;
			m_idlePathProgress = 0.0f;
		}
	}
	
	bool EnemyAI::IsPointVisible(glm::vec2 point) const
	{
		return !m_gameWorld->IsRayObstructed(point, m_entity.GetTransform().GetPosition(), ICollidable::IsObject);
	}
	
	void EnemyAI::WalkPath(float dt, const Path& path, float& progress, bool modulate)
	{
		TankGame::WalkPath(dt, path, progress, m_entity.GetTransform(), m_parameters.m_movementSpeed,
		                   glm::radians(90.0f), modulate);
	}
}
