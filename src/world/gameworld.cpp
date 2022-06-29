#include "gameworld.h"
#include "particles/particleemitter.h"
#include "entities/energyball.h"
#include "entities/explosionentity.h"
#include "entities/enemies/enemytank.h"
#include "entities/conveyorbeltentity.h"
#include "serialization/entityparsers/entityparser.h"
#include "serialization/parseutils.h"
#include "../lua/luavm.h"
#include "../graphics/spriterenderlist.h"
#include "../graphics/tilegridmaterial.h"
#include "../updateinfo.h"
#include "../utils/mathutils.h"
#include "../utils/random.h"
#include "../progress.h"
#include "../gamemanager.h"
#include "../platform/common.h"
#include "../profiling.h"

#include <imgui.h>

namespace TankGame
{
	GameWorld::GameWorld(int width, int height, Types type)
	    : m_width(width), m_height(height), m_type(type),
	      m_tileGridMaterial(&TileGridMaterial::GetInstance())
	{
		InitializeBounds(width + 1, height + 1);
	}
	
	void GameWorld::Update(const UpdateInfo& updateInfo)
	{
		FUNC_TIMER
		
		EntitiesManager::Update(updateInfo);
		
		float targetRotation = 0;
		if (m_focusEntity != nullptr)
			targetRotation = m_focusEntity->GetTransform().GetRotation();
		
		float rotationDiff = GetRotationDifference(m_cameraRotation, targetRotation);
		m_cameraRotation += rotationDiff * std::min(updateInfo.m_dt * 10, 1.0f);
	}
	
	EntityHandle GameWorld::Spawn(std::unique_ptr<Entity> entity)
	{
		entity->OnSpawned(*this);
		
		return EntitiesManager::Spawn(std::move(entity));
	}
	
	void GameWorld::OnEntityDespawn(Entity& entity)
	{
		if (m_focusEntity == &entity)
			m_focusEntity = nullptr;
	}
	
	glm::vec2 GameWorld::GetFocusLocation() const
	{
		if (m_focusEntity == nullptr)
			return { 0.0f, 0.0f };
		return m_focusEntity->GetTransform().GetPosition();
	}
	
	glm::vec2 GameWorld::GetGroundVelocity(glm::vec2 position) const
	{
		glm::vec2 groundVelocity;
		
		IterateIntersectingEntities(Rectangle::CreateCentered(position, 1, 1), [&] (const Entity& entity)
		{
			const ConveyorBeltEntity* conveyorBelt = dynamic_cast<const ConveyorBeltEntity*>(&entity);
			
			if (conveyorBelt != nullptr)
				groundVelocity += conveyorBelt->GetPushVector(position);
		});
		
		return groundVelocity;
	}
	
	ViewInfo GameWorld::GetViewInfo(float aspectRatio) const
	{
		glm::vec2 focus = GetFocusLocation();
		
		double cameraShakeTime = m_cameraShakeEnd - frameBeginTime;
		
		if (cameraShakeTime > 0)
		{
			float maxShakeMag = m_cameraShakeMagnitude * cameraShakeTime;
			std::uniform_real_distribution<float> dist(-maxShakeMag, maxShakeMag);
			
			focus.x += dist(globalRNG);
			focus.y += dist(globalRNG);
		}
		
		return ViewInfo(focus, m_cameraRotation + glm::pi<float>(), m_viewWidth, aspectRatio);
	}
	
	IntersectInfo GameWorld::GetTileIntersectInfo(const Circle& circle) const
	{
		if (m_tileGrid != nullptr && m_tileGridMaterial != nullptr)
			return m_tileGrid->GetIntersectInfo(*m_tileGridMaterial, circle);
		return { };
	}
	
	float GameWorld::GetTileRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const
	{
		if (m_tileGrid == nullptr || m_tileGridMaterial == nullptr)
			return std::numeric_limits<float>::quiet_NaN();
		return m_tileGrid->GetDistanceToWall(*m_tileGridMaterial, start, direction);
	}
	
	void GameWorld::CheckIntersection(const ICollidable& collidable, const Circle& circle,
	                                  IntersectInfo& intersectInfo) const
	{
		IntersectInfo collidableIntersectInfo = collidable.GetColliderInfo().GetIntersectInfo(circle);
		
		if (collidableIntersectInfo.m_intersects)
		{
			if (intersectInfo.m_intersects)
				intersectInfo.m_penetration += collidableIntersectInfo.m_penetration;
			else
				intersectInfo = collidableIntersectInfo;
		}
	}
	
	void GameWorld::CheckRayIntersection(const ICollidable& collidable, glm::vec2 start, glm::vec2 dir,
	                                     float& dist) const
	{
		float intersectDist = collidable.GetColliderInfo().GetRayIntersectionDistance(start, dir);
		if (!std::isnan(intersectDist) && (std::isnan(dist) || intersectDist < dist))
			dist = intersectDist;
	}
	
	void GameWorld::ShakeCamera(double time, float amount)
	{
		m_cameraShakeEnd = frameBeginTime + time;
		m_cameraShakeMagnitude = amount / time;
	}
	
	bool GameWorld::SetCheckpoint(int index, glm::vec2 position, float rotation)
	{
		if (m_currentCheckpointIndex > index)
			return false;
		m_currentCheckpointIndex = index;
		
		if (!m_progressLevelName.empty())
			Progress::GetInstance().UpdateLevelProgress(m_progressLevelName, index);
		
		m_respawnPosition = position;
		m_respawnRotation = rotation;
		
		return true;
	}
	
	void GameWorld::InitLuaSandbox(lua_State* state)
	{
		// ** findEntity **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			Entity* entity = GetEntityByName(lua_tostring(state, 1));
			
			if (entity != nullptr)
				entity->PushLuaInstance(state);
			else
				lua_pushnil(state);
			
			return 1;
		});
		lua_setfield(state, -2, "findEntity");
		
		// ** setShowBossHP **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			if (m_gameManager != nullptr)
				m_gameManager->SetShowGlobalHealthBar(lua_toboolean(state, 1));
			return 0;
		});
		lua_setfield(state, -2, "setShowBossHP");
		
		// ** setBossHP **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			if (m_gameManager != nullptr)
				m_gameManager->SetGlobalHealthBarPercentage(static_cast<float>(luaL_checknumber(state, 1)));
			return 0;
		});
		lua_setfield(state, -2, "setBossHP");
		
		// ** levelComplete **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			if (m_gameManager == nullptr)
				return 0;
			
			std::string nextLevel;
			if (lua_gettop(state) > 0)
				nextLevel = lua_tostring(state, 1);
			
			m_gameManager->LevelComplete(std::move(nextLevel));
			
			return 0;
		});
		lua_setfield(state, -2, "levelComplete");
		
		// ** explosion **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			const glm::vec2 pos(luaL_checknumber(state, 1), luaL_checknumber(state, 2));
			
			auto entity = std::make_unique<ExplosionEntity>(GetParticlesManager());
			entity->GetTransform().SetPosition(pos);
			Spawn(std::move(entity));
			
			return 0;
		});
		lua_setfield(state, -2, "explosion");
		
		// ** spawnEnergyBall **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			const glm::vec2 pos(luaL_checknumber(state, 1), luaL_checknumber(state, 2));
			const glm::vec2 dir(luaL_checknumber(state, 3), luaL_checknumber(state, 4));
			float damage = luaL_optnumber(state, 5, 70);
			
			auto entity = std::make_unique<EnergyBall>(dir, damage, GetParticlesManager());
			entity->GetTransform().SetPosition(pos);
			
			Entity* entityCopy = entity.get();
			Spawn(std::move(entity));
			entityCopy->PushLuaInstance(state);
			
			return 1;
		});
		lua_setfield(state, -2, "spawnEnergyBall");
		
		// ** spawnEnemyTank **
		Lua::PushFunction(state, [this] (lua_State* state) -> int
		{
			const glm::vec2 pos(luaL_checknumber(state, 1), luaL_checknumber(state, 2));
			
			//Parses parameters from argument #3
			bool hasShield = false;
			bool isRocketTank = false;
			float hp = 50;
			if (lua_istable(state, 3))
			{
				lua_getfield(state, 3, "hasShield");
				if (!lua_isnil(state, -1))
					hasShield = lua_toboolean(state, -1);
				
				lua_getfield(state, 3, "isRocketTank");
				if (!lua_isnil(state, -1))
					isRocketTank = lua_toboolean(state, -1);
				
				lua_getfield(state, 3, "hp");
				if (!lua_isnil(state, -1))
					hp = std::max(static_cast<float>(lua_tonumber(state, -1)), 10.0f);
				
				lua_pop(state, 3);
			}
			
			Path idlePath;
			if (lua_istable(state, 4))
			{
				lua_len(state, 4);
				lua_Integer pathLen = lua_tointeger(state, -1);
				lua_pop(state, 1);
				
				for (lua_Integer i = 1; i <= pathLen; i++)
				{
					lua_geti(state, 4, i); //Fetches the coordinate array
					
					lua_geti(state, -1, 1); //Fetches the x-coordinate from the coordinate array
					float x = static_cast<float>(lua_tonumber(state, -1));
					lua_geti(state, -2, 2); //Fetches the y-coordinate from the coordinate array
					float y = static_cast<float>(lua_tonumber(state, -1));
					
					idlePath.AddNode({ x, y });
					
					lua_pop(state, 3);
				}
			}
			else
			{
				idlePath.AddNode(pos + glm::vec2(0.1f, 0.1f));
				idlePath.AddNode(pos + glm::vec2(-0.1f, 0.1f));
				idlePath.AddNode(pos + glm::vec2(-0.1f, -0.1f));
				idlePath.AddNode(pos + glm::vec2(0.1f, -0.1f));
			}
			idlePath.Close();
			
			std::unique_ptr<EnemyTank> entity = std::make_unique<EnemyTank>(std::move(idlePath));
			entity->SetHasShield(hasShield);
			entity->SetIsRocketTank(isRocketTank);
			entity->SetHp(hp);
			
			EnemyTank* entityCopy = entity.get();
			Spawn(std::move(entity));
			
			entityCopy->GetTransform().SetPosition(pos); //We have to set this again because onspawned moved the entity onto it's idle path
			entityCopy->DetectPlayer();
			entityCopy->PushLuaInstance(state);
			
			return 1;
		});
		lua_setfield(state, -2, "spawnEnemyTank");
	}
	
	void GameWorld::SetLuaSandbox(const Lua::Sandbox* sandbox)
	{
		m_luaSandbox = sandbox;
	}
	
	void GameWorld::SetFocusEntity(const Entity* entity)
	{
		m_focusEntity = entity;
		if (entity != nullptr)
			m_cameraRotation = entity->GetTransform().GetRotation();
	}
	
	void GameWorld::SetTileGrid(std::unique_ptr<TileGrid>&& tileGrid)
	{
		m_tileGrid = std::move(tileGrid);
		UpdateTileShadowCasters();
	}
	
	void GameWorld::SetTileGridMaterial(const TileGridMaterial* tileGridMaterial)
	{
		m_tileGridMaterial = tileGridMaterial;
		UpdateTileShadowCasters();
	}
	
	void GameWorld::UpdateTileShadowCasters()
	{
		if (m_tileGrid != nullptr && m_tileGridMaterial != nullptr)
			m_tileShadowCastersBuffer = std::make_unique<TileShadowCastersBuffer>(*m_tileGrid, *m_tileGridMaterial);
		else
			m_tileShadowCastersBuffer = nullptr;
	}
}
