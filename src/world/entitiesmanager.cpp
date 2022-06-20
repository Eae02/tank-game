#include "entitiesmanager.h"
#include "particles/particleemitter.h"
#include "../updateinfo.h"
#include "../platform/common.h"

namespace TankGame
{
	EntityHandle EntitiesManager::Spawn(std::unique_ptr<Entity> entity)
	{
		if (Entity::IUpdateable* updateable = entity->AsUpdatable())
			m_updateableEntities.push_back(updateable);
		if (const ICollidable* collidable = entity->AsCollidable())
			m_collidableEntities.push_back(collidable);
		if (ParticleSystemEntityBase* psEntity = dynamic_cast<ParticleSystemEntityBase*>(entity.get()))
			m_particleSystemEntities.push_back(psEntity);
		
		uint64_t id = m_nextEntityID++;
		m_entities.emplace_back(std::move(entity), id);
		
		return EntityHandle(*this, id, m_entities.size() - 1);
	}
	
	void EntitiesManager::Update(const UpdateInfo& updateInfo)
	{
		m_particlesManager.Update(updateInfo.m_dt);
		
		if (!updateInfo.m_isEditorOpen)
		{
			for (long i = m_updateableEntities.size() - 1; i >= 0; i--)
				m_updateableEntities[i]->Update(updateInfo);
		}
		
		if (!m_entitiesToDespawn.empty())
		{
			for (size_t i = 0; i < m_entities.size();)
			{
				bool shouldDespawn = std::find(m_entitiesToDespawn.begin(), m_entitiesToDespawn.end(),
				                               m_entities[i].m_entity.get()) != m_entitiesToDespawn.end();
				
				if (shouldDespawn)
					DespawnAtIndex(i);
				else
					i++;
			}
			
			m_entitiesToDespawn.clear();
		}
		
		for (ParticleSystemEntityBase* psEntity : m_particleSystemEntities)
		{
			if (frameBeginTime > psEntity->GetDeathTime())
			{
				bool hasParticles = false;
				for (long i = static_cast<long>(psEntity->GetParticleSystem().GetEmitterCount()) - 1; i >= 0; i--)
				{
					if (psEntity->GetParticleSystem().GetEmitter(i)->HasParticles())
					{
						hasParticles = true;
						break;
					}
				}
				
				if (!hasParticles)
				{
					psEntity->Despawn();
					continue;
				}
			}
			else if (psEntity->IsEnabled())
			{
				for (long i = static_cast<long>(psEntity->GetParticleSystem().GetEmitterCount()) - 1; i >= 0; i--)
				{
					psEntity->GetParticleSystem().GetEmitter(i)->SpawnParticles(updateInfo.m_dt);
				}
			}
			
			psEntity->UpdateLastFrameTransform();
		}
	}
	
	Entity* EntitiesManager::GetEntityByName(const std::string& name)
	{
		for (const EntityEntry& entity : m_entities)
		{
			if (entity.m_entity->GetName() == name)
				return entity.m_entity.get();
		}
		return nullptr;
	}
	
	template <typename Tp>
	static void MaybeRemoveFromEntityList(Tp* entity, std::vector<Tp*>& vec)
	{
		if (entity == nullptr)
			return;
		
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec[i] == entity)
			{
				vec[i] = vec.back();
				vec.pop_back();
			}
		}
	}
	
	void EntitiesManager::DespawnAtIndex(size_t index)
	{
		m_entities[index].m_entity->OnDespawning();
		
		MaybeRemoveFromEntityList(m_entities[index].m_entity->AsUpdatable(), m_updateableEntities);
		MaybeRemoveFromEntityList(m_entities[index].m_entity->AsCollidable(), m_collidableEntities);
		MaybeRemoveFromEntityList(dynamic_cast<ParticleSystemEntityBase*>(m_entities[index].m_entity.get()),
		                          m_particleSystemEntities);
		
		OnEntityDespawn(*m_entities[index].m_entity);
		
		m_entities[index].Swap(m_entities.back());
		m_entities.pop_back();
	}
	
	long EntitiesManager::GetEntityIndexById(uint64_t id)
	{
		auto pos = std::find_if(m_entities.begin(), m_entities.end(), [&](const EntityEntry& entry)
		{
			return entry.m_id == id;
		});
		
		if (pos == m_entities.end())
			return -1;
		return pos - m_entities.begin();
	}
	
	void EntitiesManager::EntityEntry::Swap(EntitiesManager::EntityEntry& other)
	{
		m_entity.swap(other.m_entity);
		std::swap(m_id, other.m_id);
	}
}
