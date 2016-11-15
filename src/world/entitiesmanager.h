#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "entities/particlesystementity.h"
#include "entity.h"
#include "entityhandle.h"

namespace TankGame
{
	class EntitiesManager
	{
		friend class EntityHandle;
		
	public:
		virtual EntityHandle Spawn(std::unique_ptr<Entity>&& entity);
		EntityHandle SpawnNamed(std::unique_ptr<Entity>&& entity, const std::string& name);
		
		inline void Despawn(Entity* entity)
		{
			m_entitiesToDespawn.push_back(entity);
		}
		
		void Update(const class UpdateInfo& updateInfo);
		
		Entity* GetEntityByName(const std::string& name);
		inline const Entity* GetEntityByName(const std::string& name) const
		{ return const_cast<EntitiesManager*>(this)->GetEntityByName(name); }
		
		std::string GetEntityName(const Entity& entity) const;
		
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
		void IterateParticleEmitters(CallbackTp callback) const
		{
			for (ParticleSystemEntityBase* psEntity : m_particleSystemEntities)
			{
				for (long i = psEntity->GetParticleSystem().GetEmitterCount() - 1; i >= 0; i--)
					callback(*psEntity->GetParticleSystem().GetEmitter(i));
			}
		}
		
	protected:
		virtual void OnEntityDespawn(Entity& entity) { }
		
	private:
		void DespawnAtIndex(size_t index);
		
		long GetEntityIndexById(uint64_t id);
		
		struct EntityEntry
		{
			std::unique_ptr<Entity> m_entity;
			uint64_t m_id;
			
			void Swap(EntityEntry& other);
			
			inline EntityEntry(std::unique_ptr<Entity>&& entity, uint64_t id)
			    : m_entity(std::move(entity)), m_id(id) { }
		};
		
		uint64_t m_nextEntityID = 0;
		
		std::vector<EntityEntry> m_entities;
		
		std::unordered_map<std::string, Entity*> m_namedEntities;
		
		std::vector<Entity*> m_entitiesToDespawn;
		
		std::vector<Entity::IUpdateable*> m_updateableEntities;
		std::vector<ParticleSystemEntityBase*> m_particleSystemEntities;
	};
}
