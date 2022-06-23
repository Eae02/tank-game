#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "particles/particlesmanager.h"
#include "entities/particlesystementity.h"
#include "entity.h"
#include "entityhandle.h"
#include "icollidable.h"

namespace TankGame
{
	class EntitiesManager
	{
		friend class EntityHandle;
		
	public:
		virtual EntityHandle Spawn(std::unique_ptr<Entity> entity);
		
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
					for (const std::vector<EntityHandle>* entityList : { &reg.staticEntities, &reg.dynamicEntities })
					{
						for (EntityHandle handle : *entityList)
						{
							handle.UpdateLastIndex();
							if (handle.m_lastIndex != -1 &&
								m_entities[handle.m_lastIndex].m_iterateIntersectingID < m_nextIterateIntersectingID &&
								m_entities[handle.m_lastIndex].m_entity->GetBoundingRectangle().Intersects(rectangle))
							{
								m_entities[handle.m_lastIndex].m_iterateIntersectingID = m_nextIterateIntersectingID;
								callback(*m_entities[handle.m_lastIndex].m_entity);
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
			for (ParticleSystemEntityBase* psEntity : m_particleSystemEntities)
			{
				for (long i = psEntity->GetParticleSystem().GetEmitterCount() - 1; i >= 0; i--)
					callback(*psEntity->GetParticleSystem().GetEmitter(i));
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
			uint64_t m_id;
			uint64_t m_iterateIntersectingID = 0;
			
			void Swap(EntityEntry& other);
			
			inline EntityEntry(std::unique_ptr<Entity>&& entity, uint64_t id)
			    : m_entity(std::move(entity)), m_id(id) { }
		};
		
		uint64_t m_nextEntityID = 0;
		
		ParticlesManager m_particlesManager;
		
		static constexpr float REGION_SIZE = 8;
		
		struct Region
		{
			std::vector<EntityHandle> staticEntities;
			std::vector<EntityHandle> dynamicEntities;
			std::vector<EntityHandle> dynamicEntities2;
		};
		std::vector<Region> m_regions;
		uint32_t m_numRegionsX = 0;
		uint32_t m_numRegionsY = 0;
		
		uint64_t m_nextIterateIntersectingID = 0;
		
		typedef std::vector<EntityHandle> Region::*RegionField;
		
		std::pair<glm::ivec2, glm::ivec2> GetRegionBounds(const Rectangle& rectangle);
		void PushToRegions(EntityHandle handle, std::pair<glm::ivec2, glm::ivec2> regionBounds, RegionField field);
		
		std::vector<EntityEntry> m_entities;
		
		std::vector<Entity*> m_entitiesToDespawn;
		
		std::vector<EntityHandle> m_updateableEntities;
		std::vector<ParticleSystemEntityBase*> m_particleSystemEntities;
		
		std::vector<EntityUpdateTimeStatistics> m_updateTimeStatistics;
	};
}
