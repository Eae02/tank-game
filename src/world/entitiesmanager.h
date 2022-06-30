#pragma once

#include "particles/particlesmanager.h"
#include "entities/particlesystementity.h"
#include "entity.h"
#include "entityhandle.h"
#include "icollidable.h"

#include <unordered_map>

namespace TankGame
{
	class EntitiesManager
	{
		friend class EntityHandleData;
		
	public:
		virtual EntityHandle<> Spawn(std::unique_ptr<Entity> entity);
		
		template <typename T>
		EntityHandle<T> SpawnT(std::unique_ptr<T> entity)
		{
			T* entityPtr = entity.get();
			EntityHandle<> genericHandle = Spawn(std::move(entity));
			return EntityHandle<T>(genericHandle.m_data, entityPtr);
		}
		
		inline void Despawn(Entity* entity)
		{
			m_entitiesToDespawn.push_back(entity);
		}
		
		void Update(const class UpdateInfo& updateInfo);
		
		inline ParticlesManager& GetParticlesManager()
		{ return m_particlesManager; }
		
		Entity* GetEntityByName(const std::string& name);
		inline const Entity* GetEntityByName(const std::string& name) const
		{ return const_cast<EntitiesManager*>(this)->GetEntityByName(name); }
		
		template <typename CallbackTp>
		void IterateEntities(CallbackTp callback) const
		{
			for (const EntityEntry& entity : m_entities)
				callback(const_cast<const Entity&>(*entity.m_entity));
		}
		
		template <typename CallbackTp>
		void IterateEntities(CallbackTp callback)
		{
			for (const EntityEntry& entity : m_entities)
				callback(*entity.m_entity);
		}
		
		template <typename CallbackTp>
		void IterateIntersectingEntities(const Rectangle& rectangle, CallbackTp callback)
		{
			m_nextIterateIntersectingID++;
			auto [lo, hi] = GetRegionBounds(rectangle);
			for (int y = lo.y; y <= hi.y; y++)
			{
				for (int x = lo.x; x <= hi.x; x++)
				{
					const Region& reg = m_regions[y * m_numRegionsX + x];
					for (const std::vector<EntityHandleData>* entityList : { &reg.staticEntities, &reg.dynamicEntities })
					{
						for (EntityHandleData handle : *entityList)
						{
							if (!handle.CheckValidity())
								continue;
							
							if (m_entities[handle.GetLastIndex()].m_iterateIntersectingID < m_nextIterateIntersectingID &&
								m_entities[handle.GetLastIndex()].m_entity->GetBoundingRectangle().Intersects(rectangle))
							{
								m_entities[handle.GetLastIndex()].m_iterateIntersectingID = m_nextIterateIntersectingID;
								callback(*m_entities[handle.GetLastIndex()].m_entity);
							}
						}
					}
				}
			}
		}
		
		template <typename CallbackTp>
		void IterateIntersectingEntities(const Rectangle& rectangle, CallbackTp callback) const
		{
			const_cast<EntitiesManager*>(this)->IterateIntersectingEntities(
				rectangle, [&] (Entity& ent) { callback(const_cast<const Entity&>(ent)); });
		}
		
		template <typename CallbackTp>
		void IterateParticleEmitters(CallbackTp callback) const
		{
			for (EntityHandle<ParticleSystemEntityBase> entityHandle : m_particleSystemEntities)
			{
				if (const ParticleSystemEntityBase* psEntity = entityHandle.Get())
				{
					for (long i = psEntity->GetParticleSystem().GetEmitterCount() - 1; i >= 0; i--)
						callback(*psEntity->GetParticleSystem().GetEmitter(i));
				}
			}
		}
		
		struct EntityUpdateTimeStatistics
		{
			double totalTime;
			int count;
			std::string_view typeName;
		};
		
		bool collectUpdateTimeStatistics = false;
		
		const std::vector<EntityUpdateTimeStatistics>& GetUpdateTimeStatistics() const
		{ return m_updateTimeStatistics; }
		
		void InitializeBounds(float width, float height);
		
	protected:
		virtual void OnEntityDespawn(Entity& entity) { }
		
	private:
		void DespawnAtIndex(size_t index);
		
		long GetEntityIndexById(uint64_t id);
		
		struct EntityEntry
		{
			std::unique_ptr<Entity> m_entity;
			uint64_t m_iterateIntersectingID = 0;
		};
		
		uint64_t m_nextEntityID = 0;
		
		ParticlesManager m_particlesManager;
		
		static constexpr float REGION_SIZE = 8;
		
		struct Region
		{
			std::vector<EntityHandleData> staticEntities;
			std::vector<EntityHandleData> dynamicEntities;
			std::vector<EntityHandleData> dynamicEntities2;
		};
		std::vector<Region> m_regions;
		uint32_t m_numRegionsX = 0;
		uint32_t m_numRegionsY = 0;
		
		uint64_t m_nextIterateIntersectingID = 0;
		
		typedef std::vector<EntityHandleData> Region::*RegionField;
		
		std::pair<glm::ivec2, glm::ivec2> GetRegionBounds(const Rectangle& rectangle);
		void PushToRegions(EntityHandleData handle, std::pair<glm::ivec2, glm::ivec2> regionBounds, RegionField field);
		
		std::vector<EntityEntry> m_entities;
		
		std::unordered_map<uint64_t, size_t> m_entityIDToIndex;
		
		std::vector<Entity*> m_entitiesToDespawn;
		
		std::vector<EntityHandle<Entity::IUpdateable>> m_updateableEntities;
		std::vector<EntityHandle<ParticleSystemEntityBase>> m_particleSystemEntities;
		
		std::vector<EntityUpdateTimeStatistics> m_updateTimeStatistics;
	};
}
