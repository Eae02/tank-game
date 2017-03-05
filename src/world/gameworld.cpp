#include "gameworld.h"
#include "particles/particleemitter.h"
#include "entities/conveyorbeltentity.h"
#include "../lua/luavm.h"
#include "../graphics/spriterenderlist.h"
#include "../graphics/tilegridmaterial.h"
#include "../updateinfo.h"
#include "../utils/mathutils.h"
#include "../utils/utils.h"
#include "../progress.h"

#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <limits>
#include <imgui.h>

namespace TankGame
{
	GameWorld::GameWorld(int width, int height, Types type)
	    : m_width(width), m_height(height), m_type(type),
	      m_quadTree(Rectangle(0, 0, width, height)), m_tileGridMaterial(&TileGridMaterial::GetInstance())
	{
		
	}
	
	void GameWorld::Update(const UpdateInfo& updateInfo)
	{
		EntitiesManager::Update(updateInfo);
		
		m_particlesManager.Update(updateInfo.m_dt);
		
		float targetRotation = 0;
		if (m_focusEntity != nullptr)
			targetRotation = m_focusEntity->GetTransform().GetRotation();
		
		float rotationDiff = GetRotationDifference(m_cameraRotation, targetRotation);
		m_cameraRotation += rotationDiff * std::min(updateInfo.m_dt * 10, 1.0f);
		
		m_quadTree.Update();
	}
	
	EntityHandle GameWorld::Spawn(std::unique_ptr<Entity>&& entity)
	{
		m_quadTree.Add(*entity);
		
		entity->OnSpawned(*this);
		
		return EntitiesManager::Spawn(std::move(entity));
	}
	
	void GameWorld::OnEntityDespawn(Entity& entity)
	{
		if (m_focusEntity == &entity)
			m_focusEntity = nullptr;
		
		m_quadTree.Remove(entity);
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
		
		double cameraShakeTime = m_cameraShakeEnd - glfwGetTime();
		
		if (cameraShakeTime > 0)
		{
			float maxShakeMag = m_cameraShakeMagnitude * cameraShakeTime;
			std::uniform_real_distribution<float> dist(-maxShakeMag, maxShakeMag);
			
			focus.x += dist(randomGen);
			focus.y += dist(randomGen);
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
		m_cameraShakeEnd = glfwGetTime() + time;
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
