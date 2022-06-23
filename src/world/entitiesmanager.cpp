#include "entitiesmanager.h"
#include "particles/particleemitter.h"
#include "../updateinfo.h"
#include "../platform/common.h"
#include "../profiling.h"

namespace TankGame
{
	EntityHandle EntitiesManager::Spawn(std::unique_ptr<Entity> entity)
	{
		if (ParticleSystemEntityBase* psEntity = dynamic_cast<ParticleSystemEntityBase*>(entity.get()))
			m_particleSystemEntities.push_back(psEntity);
		
		uint64_t id = m_nextEntityID++;
		
		EntityHandle handle(*this, id, m_entities.size());
		
		bool canMove = false;
		if (Entity::IUpdateable* updateable = entity->AsUpdatable())
		{
			canMove = updateable->CanMoveDuringUpdate();
			m_updateableEntities.push_back(handle);
		}
		
		PushToRegions(
			handle, GetRegionBounds(entity->GetBoundingRectangle()),
			canMove ? &Region::dynamicEntities : &Region::staticEntities);
		
		m_entities.emplace_back(std::move(entity), id);
		
		return handle;
	}
	
	std::pair<glm::ivec2, glm::ivec2> EntitiesManager::GetRegionBounds(const Rectangle& rectangle)
	{
		int lox = glm::clamp((int)std::floor(rectangle.x / REGION_SIZE), 0, (int)m_numRegionsX - 1);
		int loy = glm::clamp((int)std::floor(rectangle.y / REGION_SIZE), 0, (int)m_numRegionsY - 1);
		int hix = glm::clamp((int)std::floor(rectangle.FarX() / REGION_SIZE), 0, (int)m_numRegionsX - 1);
		int hiy = glm::clamp((int)std::floor(rectangle.FarY() / REGION_SIZE), 0, (int)m_numRegionsY - 1);
		return std::make_pair(glm::ivec2(lox, loy), glm::ivec2(hix, hiy));
	}
	
	void EntitiesManager::PushToRegions(EntityHandle handle, std::pair<glm::ivec2, glm::ivec2> regionBounds, RegionField field)
	{
		for (int y = regionBounds.first.y; y <= regionBounds.second.y; y++)
		{
			for (int x = regionBounds.first.x; x <= regionBounds.second.x; x++)
			{
				(m_regions[y * m_numRegionsX + x].*field).push_back(handle);
			}
		}
	}
	
	void EntitiesManager::InitializeBounds(float width, float height)
	{
		m_numRegionsX = (uint32_t)std::ceil(width / REGION_SIZE);
		m_numRegionsY = (uint32_t)std::ceil(height / REGION_SIZE);
		m_regions.resize(m_numRegionsX * m_numRegionsY);
	}
	
	void EntitiesManager::Update(const UpdateInfo& updateInfo)
	{
		FUNC_TIMER
		
		m_particlesManager.Update(updateInfo.m_dt);
		
		if (!updateInfo.m_isEditorOpen)
		{
			SCOPE_TIMER("Update Entities")
			
			std::unordered_map<std::string_view, EntityUpdateTimeStatistics> updateTimes;
			
			for (long i = m_updateableEntities.size() - 1; i >= 0; i--)
			{
				Entity* entity = m_updateableEntities[i].Get();
				if (!entity)
				{
					m_updateableEntities[i] = m_updateableEntities.back();
					m_updateableEntities.pop_back();
					continue;
				}
				
				Entity::IUpdateable* updatable = entity->AsUpdatable();
				if (!updatable)
					continue;
				
				double startTime;
				if (collectUpdateTimeStatistics)
					startTime = GetTime();
				
				updatable->Update(updateInfo);
				
				if (updatable->CanMoveDuringUpdate())
				{
					PushToRegions(m_updateableEntities[i], GetRegionBounds(entity->GetBoundingRectangle()), &Region::dynamicEntities2);
				}
				
				if (collectUpdateTimeStatistics)
				{
					double endTime = GetTime();
					std::string_view name = typeid(*entity).name();
					auto it = updateTimes.emplace(name, EntityUpdateTimeStatistics { }).first;
					it->second.typeName = name;
					it->second.count++;
					it->second.totalTime += endTime - startTime;
				}
			}
			
			if (collectUpdateTimeStatistics)
			{
				m_updateTimeStatistics.clear();
				for (const auto& x : updateTimes)
				{
					m_updateTimeStatistics.push_back(x.second);
				}
			}
		}
		
		bool despawnAny = !m_entitiesToDespawn.empty();
		if (despawnAny)
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
		
		for (Region& region : m_regions)
		{
			region.dynamicEntities2.swap(region.dynamicEntities);
			region.dynamicEntities2.clear();
			
			if (despawnAny)
			{
				for (int64_t i = (int64_t)region.staticEntities.size() - 1; i >= 0; i--)
				{
					if (!region.staticEntities[i].IsAlive())
					{
						region.staticEntities[i] = region.staticEntities.back();
						region.staticEntities.pop_back();
					}
				}
			}
		}
		
		SCOPE_TIMER("Spawn Particles")
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
