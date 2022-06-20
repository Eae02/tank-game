#pragma once

#include "entity.h"
#include "quadtree/quadtree.h"
#include "tilegrid.h"
#include "icollidable.h"
#include "intersectinfo.h"
#include "entitiesmanager.h"
#include "particles/particleemitter.h"
#include "entities/particlesystementity.h"
#include "../graphics/tileshadowcastersbuffer.h"
#include "../graphics/viewinfo.h"
#include "../lua/sandbox.h"
#include "../lua/luainc.h"

#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <limits>

namespace TankGame
{
	class GameWorld final : public EntitiesManager
	{
	public:
		enum class Types
		{
			Game,
			Editor,
			MenuBackground,
			ScreenShot
		};
		
		GameWorld(int width, int height, Types type);
		
		//No move constructors because the game world cannot move in memory.
		GameWorld(GameWorld&& other) = delete;
		GameWorld& operator=(GameWorld&& other) = delete;
		
		void Update(const class UpdateInfo& updateInfo);
		
		ViewInfo GetViewInfo(float aspectRatio) const;
		
		virtual EntityHandle Spawn(std::unique_ptr<Entity> entity) final override;
		
		void SetFocusEntity(const Entity* entity);
		glm::vec2 GetFocusLocation() const;
		
		glm::vec2 GetGroundVelocity(glm::vec2 position) const;
		
		template <typename CallbackTp>
		void IterateIntersectingEntities(const Rectangle& rectangle, CallbackTp callback) const
		{
			m_quadTree.IterateIntersecting([&] (const IQuadTreeEntry& entry)
			{
				callback(static_cast<const Entity&>(entry));
			}, rectangle);
		}
		
		template <typename CallbackTp>
		void IterateIntersectingEntities(const Rectangle& rectangle, CallbackTp callback)
		{
			m_quadTree.IterateIntersecting([&] (const IQuadTreeEntry& entry)
			{
				callback(const_cast<Entity&>(static_cast<const Entity&>(entry)));
			}, rectangle);
		}
		
		inline Types GetWorldType() const
		{ return m_type; }
		
		inline float GetViewWidth() const
		{ return m_viewWidth; }
		
		void SetTileGrid(std::unique_ptr<TileGrid>&& tileGrid);
		void SetTileGridMaterial(const class TileGridMaterial* tileGridMaterial);
		
		void UpdateTileShadowCasters();
		
		// ** Intersections and collision detection **
		inline bool IsRayObstructed(glm::vec2 start, glm::vec2 end)
		{ return IsRayObstructed(start, end, [] (const ICollidable&) { return true; }); }
		
		template <typename IgnoreCBType>
		bool IsRayObstructed(glm::vec2 start, glm::vec2 end, IgnoreCBType ignoreCB) const
		{
			float dist = glm::distance(start, end);
			float intersectDist = GetRayIntersectionDistance(start, (end - start) / dist, ignoreCB);
			return !std::isnan(intersectDist) && intersectDist < dist;
		}
		
		IntersectInfo GetTileIntersectInfo(const Circle& circle) const;
		
		inline IntersectInfo GetIntersectInfo(const Circle& circle)
		{
			return GetIntersectInfo(circle, [] (const ICollidable&) { return true; });
		}
		
		template <typename IgnoreCBType>
		IntersectInfo GetIntersectInfo(const Circle& circle, IgnoreCBType ignoreCB) const
		{
			IntersectInfo intersectInfo = GetTileIntersectInfo(circle);
			
			IterateIntersectingEntities(circle.GetBoundingRectangle(), [&] (const Entity& entity)
			{
				const ICollidable* collidable = entity.AsCollidable();
				if (collidable != nullptr && ignoreCB(*collidable))
					CheckIntersection(*collidable, circle, intersectInfo);
			});
			
			return intersectInfo;
		}
		
		float GetTileRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const;
		
		inline float GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const
		{
			return GetRayIntersectionDistance(start, direction, [] (const ICollidable&) { return true; });
		}
		
		template <typename IgnoreCBType>
		float GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction, IgnoreCBType includeCB) const
		{
			float dist = GetTileRayIntersectionDistance(start, direction);
			
			IterateCollidable([&] (const ICollidable& collidable)
			{
				if (includeCB(collidable))
					CheckRayIntersection(collidable, start, direction, dist);
			});
			
			return dist;
		}
		
		inline const TileShadowCastersBuffer* GetTileShadowCastersBuffer() const
		{ return m_tileShadowCastersBuffer.get(); }
		
		inline const TileGrid* GetTileGrid() const
		{ return m_tileGrid.get(); }
		inline TileGrid* GetTileGrid()
		{ return m_tileGrid.get(); }
		
		const class TileGridMaterial* GetTileGridMaterial() const
		{ return m_tileGridMaterial; }
		
		void ShakeCamera(double time, float amount);
		
		bool SetCheckpoint(int index, glm::vec2 position, float rotation);
		
		void InitLuaSandbox(lua_State* state);
		
		inline void SetProgressLevelName(std::string levelName)
		{ m_progressLevelName = std::move(levelName); }
		
		inline glm::vec2 GetRespawnPosition() const
		{ return m_respawnPosition; }
		inline float GetRespawnRotation() const
		{ return m_respawnRotation; }
		
		inline void SetRenderer(class IMainRenderer* renderer)
		{ m_renderer = renderer; }
		inline class IMainRenderer* GetRenderer() const
		{ return m_renderer; }
		
		inline void SetGameManager(class GameManager* gameManager)
		{ m_gameManager = gameManager; }
		inline class GameManager* GetGameManager() const
		{ return m_gameManager; }
		
		inline const QuadTree& GetQuadTree()
		{ return m_quadTree; }
		
		inline int GetWidth() const
		{ return m_width; }
		inline int GetHeight() const
		{ return m_height; }
		
		inline const Lua::Sandbox* GetLuaSandbox() const
		{ return m_luaSandbox; }
		void SetLuaSandbox(const Lua::Sandbox* sandbox);
		
	protected:
		virtual void OnEntityDespawn(Entity& entity) final override;
		
	private:
		//Helper function to detemplatize GetIntersectInfo
		void CheckIntersection(const ICollidable& collidable, const Circle& circle, IntersectInfo& intersectInfo) const;
		
		//Helper functions to detemplatize GetRayIntersectionDistance
		void CheckRayIntersection(const ICollidable& collidable, glm::vec2 start, glm::vec2 dir, float& dist) const;
		
		void DespawnAtIndex(size_t index);
		
		const Lua::Sandbox* m_luaSandbox = nullptr;
		
		std::string m_progressLevelName;
		
		int m_width;
		int m_height;
		
		Types m_type;
		
		int m_currentCheckpointIndex = std::numeric_limits<int>::min();
		glm::vec2 m_respawnPosition;
		float m_respawnRotation;
		
		const Entity* m_focusEntity = nullptr;
		float m_viewWidth = 20;
		
		float m_cameraRotation = 0;
		
		double m_cameraShakeEnd = 0;
		float m_cameraShakeMagnitude = 0;
		
		class IMainRenderer* m_renderer = nullptr;
		class GameManager* m_gameManager = nullptr;
		
		QuadTree m_quadTree;
		
		std::unique_ptr<TileGrid> m_tileGrid;
		const class TileGridMaterial* m_tileGridMaterial = nullptr;
		std::unique_ptr<TileShadowCastersBuffer> m_tileShadowCastersBuffer;
	};
}
