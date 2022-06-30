#include "entitiesmanager.h"
#include "particles/particleemitter.h"
#include "../updateinfo.h"
#include "../platform/common.h"
#include "../profiling.h"

namespace TankGame
{
	EntityHandle<> EntitiesManager::Spawn(std::unique_ptr<Entity> entity)
	{
		entity->m_entityID = m_nextEntityID++;
		
		EntityHandleData handleData(*this, entity->m_entityID, m_entities.size());
		
		m_entityIDToIndex.emplace(entity->m_entityID, m_entities.size());
		
		if (ParticleSystemEntityBase* psEntity = dynamic_cast<ParticleSystemEntityBase*>(entity.get()))
			m_particleSystemEntities.push_back({ handleData, psEntity });
		
		bool canMove = false;
		if (Entity::IUpdateable* updateable = entity->AsUpdatable())
		{
			canMove = updateable->CanMoveDuringUpdate();
			m_updateableEntities.push_back({ handleData, updateable });
		}
		
		PushToRegions(
			handleData, GetRegionBounds(entity->GetBoundingRectangle()),
			canMove ? &Region::dynamicEntities : &Region::staticEntities);
		
		EntityHandle<> handle(handleData, entity.get());
		m_entities.push_back(EntityEntry { std::move(entity), 0 });
		
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
	
	void EntitiesManager::PushToRegions(EntityHandleData handle, std::pair<glm::ivec2, glm::ivec2> regionBounds, RegionField field)
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
				Entity::IUpdateable* updatable = m_updateableEntities[i].Get();
				if (!updatable)
				{
					m_updateableEntities[i] = m_updateableEntities.back();
					m_updateableEntities.pop_back();
					continue;
				}
				
				Entity* entity = m_updateableEntities[i].m_data.GetEntity();
				
				double startTime = 0;
				if (collectUpdateTimeStatistics)
					startTime = GetTime();
				
				updatable->Update(updateInfo);
				
				if (updatable->CanMoveDuringUpdate())
				{
					PushToRegions(
						m_updateableEntities[i].m_data,
						GetRegionBounds(entity->GetBoundingRectangle()), &Region::dynamicEntities2);
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
					if (!region.staticEntities[i].CheckValidity())
					{
						region.staticEntities[i] = region.staticEntities.back();
						region.staticEntities.pop_back();
					}
				}
			}
		}
		
		SCOPE_TIMER("Spawn Particles")
		for (long i = (long)m_particleSystemEntities.size() - 1; i >= 0; i--)
		{
			ParticleSystemEntityBase* psEntity = m_particleSystemEntities[i].Get();
			if (!psEntity)
			{
				m_particleSystemEntities[i] = m_particleSystemEntities.back();
				m_particleSystemEntities.pop_back();
				continue;
			}
			
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
					m_particleSystemEntities[i] = m_particleSystemEntities.back();
					m_particleSystemEntities.pop_back();
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
	
	void EntitiesManager::DespawnAtIndex(size_t index)
	{
		m_entityIDToIndex.erase(m_entities[index].m_entity->m_entityID);
		
		m_entities[index].m_entity->OnDespawning();
		
		OnEntityDespawn(*m_entities[index].m_entity);
		
		if (index != m_entities.size() - 1)
		{
			m_entityIDToIndex.at(m_entities.back().m_entity->m_entityID) = index;
			m_entities[index] = std::move(m_entities.back());
		}
		m_entities.pop_back();
	}
	
	long EntitiesManager::GetEntityIndexById(uint64_t id)
	{
		auto pos = m_entityIDToIndex.find(id);
		if (pos == m_entityIDToIndex.end())
			return -1;
		return pos->second;
	}
}
