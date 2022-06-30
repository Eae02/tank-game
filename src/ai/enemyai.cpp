#include "enemyai.h"
#include "aiutils.h"
#include "../world/entities/enemies/enemytank.h"
#include "../world/gameworld.h"
#include "../world/path/pathfinder.h"
#include "../world/spteams.h"
#include "../utils/mathutils.h"
#include "../utils/utils.h"
#include "../updateinfo.h"
#include "../platform/common.h"

namespace TankGame
{
	bool EnemyAI::MaybeBeginChasing(glm::vec2 selfPosition, glm::vec2 playerPosition)
	{
		Path newPath;
		if (!FindPath(*m_gameWorld, selfPosition, playerPosition, newPath, m_parameters.m_circleRadius))
			return false;
		m_chasePathProgress = 0.0f;
		m_lastPathUpdateTime = frameBeginTime;
		m_state = States::Chasing;
		m_path = std::move(newPath);
		return true;
	}
	
	EnemyAI::EnemyAI(AIParameters parameters, Path idlePath)
	    : m_parameters(std::move(parameters)), m_idlePath(std::move(idlePath)) { }
	
	void EnemyAI::Update(EnemyTank& entity, glm::vec2 playerPosition, const UpdateInfo& updateInfo)
	{
		//if (m_state == States::Idle && glm::distance(updateInfo.m_viewInfo.GetViewRectangle().Center(), m_entity.GetTransform().GetPosition()) > 50)
		//	return;
		
		glm::vec2 toPlayer = playerPosition - entity.GetTransform().GetPosition();
		const float SIGHT_RADIUS = 8;
		const float FIRE_RADIUS = 7;
		
		const bool isPlayerVisible =
			glm::length2(toPlayer) < SIGHT_RADIUS * SIGHT_RADIUS &&
			!m_gameWorld->IsRayObstructed(playerPosition, entity.GetTransform().GetPosition(), ICollidable::IsObject);
		
		auto WalkPath = [&] (const Path& path, float& progress, bool modulate)
		{
			TankGame::WalkPath(updateInfo.m_dt, path, progress, entity.GetTransform(),
			                   m_parameters.m_movementSpeed, glm::radians(90.0f), modulate);
		};
		
		switch (m_state)
		{
			case States::Idle:
			{
				if (isPlayerVisible)
					MaybeBeginChasing(entity.GetTransform().GetPosition(), playerPosition);
				else
				{
					WalkPath(m_idlePath, m_idlePathProgress, true);
					entity.SetCannonRotation(entity.GetTransform().GetRotation());
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
				
				entity.SetCannonRotation(glm::half_pi<float>() + std::atan2(toPlayer.y, toPlayer.x));
				
				const bool withinFiringRange = glm::length2(toPlayer) < FIRE_RADIUS * FIRE_RADIUS;
				
				if (entity.CanFire(updateInfo.m_gameTime) && withinFiringRange)
				{
					if (entity.IsRocketTank())
						entity.FireRocket(35, updateInfo.m_gameTime, { });
					else
						entity.FirePlasmaGun(ParseColorHexCodeSRGB(0xFF564A), 5, updateInfo.m_gameTime, { });
				}
				
				if (frameBeginTime > m_lastPathUpdateTime + PATH_UPDATE_INTERVAL)
				{
					m_chasePathProgress = 0.0f;
					m_lastPathUpdateTime = frameBeginTime;
					
					Path newPath;
					
					if (FindPath(*m_gameWorld, entity.GetTransform().GetPosition(),
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
				
				WalkPath(m_path, m_chasePathProgress, false);
				break;
			}
			
			case States::Searching:
			{
				WalkPath(m_path, m_chasePathProgress, false);
				
				if (isPlayerVisible && MaybeBeginChasing(entity.GetTransform().GetPosition(), playerPosition))
					break;
				
				entity.SetCannonRotation(entity.GetTransform().GetRotation());
				
				if (m_chasePathProgress >= m_path.GetTotalLength())
				{
					Path newPath;
					
					auto returnPoint = m_idlePath.GetClosestPointOnPath(entity.GetTransform().GetPosition());
					
					if (!FindPath(*m_gameWorld, entity.GetTransform().GetPosition(), returnPoint.m_position, newPath,
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
				if (isPlayerVisible && MaybeBeginChasing(entity.GetTransform().GetPosition(), playerPosition))
					break;
				
				WalkPath(m_path, m_returnToIdleProgress, false);
				entity.SetCannonRotation(entity.GetTransform().GetRotation());
				
				if (m_returnToIdleProgress >= m_path.GetTotalLength())
					m_state = States::Idle;
				
				break;
			}
		}
	}
	
	void EnemyAI::DetectPlayer(EnemyTank& entity, glm::vec2 playerPosition)
	{
		if (m_state == States::Idle || m_state == States::ReturningToIdle)
		{
			Path path;
			
			if (FindPath(*m_gameWorld, entity.GetTransform().GetPosition(), playerPosition, path,
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
}
