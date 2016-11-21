#pragma once

#include "entity.h"
#include "quadtree/quadtree.h"
#include "tilegrid.h"
#include "icollidable.h"
#include "intersectinfo.h"
#include "entitiesmanager.h"
#include "particles/particlesmanager.h"
#include "particles/particleemitter.h"
#include "entities/particlesystementity.h"
#include "../graphics/tileshadowcastersbuffer.h"
#include "../graphics/viewinfo.h"

#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <limits>

namespace TankGame
{
	class GameWorld : public EntitiesManager
	{
	public:
		class IEventListener
		{
		public:
			virtual void HandleEvent(const std::string& event, Entity* sender) = 0;
		};
		
		enum class Types
		{
			Game,
			Editor,
			MenuBackground
		};
		
		GameWorld(int width, int height, Types type);
		
		//No move constructors because the game world cannot move in memory.
		GameWorld(GameWorld&& other) = delete;
		GameWorld& operator=(GameWorld&& other) = delete;
		
		void Update(const class UpdateInfo& updateInfo);
		
		ViewInfo GetViewInfo(float aspectRatio) const;
		
		virtual EntityHandle Spawn(std::unique_ptr<Entity>&& entity) final override;
		
		void SetFocusEntity(const Entity* entity);
		glm::vec2 GetFocusLocation() const;
		
		void SendEvent(const std::string& event, Entity* sender);
		void ListenForEvent(std::string event, Entity& receiver);
		
		inline void SetEventListener(IEventListener* eventListener)
		{ m_eventListener = eventListener; }
		
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
		{ return m_tileShadowCastersBuffer.Get(); }
		
		inline const TileGrid* GetTileGrid() const
		{ return m_tileGrid.get(); }
		inline TileGrid* GetTileGrid()
		{ return m_tileGrid.get(); }
		
		const class TileGridMaterial* GetTileGridMaterial() const
		{ return m_tileGridMaterial; }
		
		inline ParticlesManager& GetParticlesManager()
		{ return m_particlesManager; }
		
		void ShakeCamera(double time, float amount);
		
		void SetCheckpoint(int index, glm::vec2 position, float rotation);
		
		inline glm::vec2 GetRespawnPosition() const
		{ return m_respawnPosition; }
		inline float GetRespawnRotation() const
		{ return m_respawnRotation; }
		
		inline void SetRenderer(class IMainRenderer* renderer)
		{ m_renderer = renderer; }
		inline class IMainRenderer* GetRenderer() const
		{ return m_renderer; }
		
		inline const QuadTree& GetQuadTree()
		{ return m_quadTree; }
		
		nlohmann::json Serialize() const;
		
	protected:
		virtual void OnEntityDespawn(Entity& entity) final override;
		
	private:
		//Helper function to detemplatize GetIntersectInfo
		void CheckIntersection(const ICollidable& collidable, const Circle& circle, IntersectInfo& intersectInfo) const;
		
		//Helper functions to detemplatize GetRayIntersectionDistance
		void CheckRayIntersection(const ICollidable& collidable, glm::vec2 start, glm::vec2 dir, float& dist) const;
		
		struct EventListenerEntry
		{
			std::string m_eventName;
			Entity* m_receiver;
			
			inline EventListenerEntry(std::string eventName, Entity& entity)
			    : m_eventName(std::move(eventName)), m_receiver(&entity) { }
		};
		
		void DespawnAtIndex(size_t index);
		
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
		
		IEventListener* m_eventListener = nullptr;
		
		QuadTree m_quadTree;
		
		ParticlesManager m_particlesManager;
		
		std::vector<EventListenerEntry> m_eventListeners;
		
		std::unique_ptr<TileGrid> m_tileGrid;
		const class TileGridMaterial* m_tileGridMaterial = nullptr;
		StackObject<TileShadowCastersBuffer> m_tileShadowCastersBuffer;
	};
}
