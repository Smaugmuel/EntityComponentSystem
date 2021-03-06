#include "pch_ECS.hpp"
#include "ECSManager.hpp"

namespace ECS
{
	ECSManager::~ECSManager()
	{
		for (auto pool : m_componentPools)
		{
			delete pool;
		}
	}

	[[nodiscard]] Entity ECSManager::createEntity()
	{
		EntityID entityID = 0;

		if (hasInvalidEntities())
		{
			entityID = getAndPopLastInvalidEntityID();
			resetAndValidateEntity(entityID);
		}
		else
		{
			entityID = createNewEntity();
		}

		return Entity(entityID);
	}
	[[nodiscard]] Bitmask ECSManager::getComponentMask(const EntityID entityID) const
	{
		return m_componentMasks[entityID];
	}
	[[nodiscard]] bool ECSManager::isValid(const EntityID entityID) const
	{
		return m_isValidEntity[entityID];
	}

	void ECSManager::reserveEntities(const size_t COUNT)
	{
		m_componentMasks.reserve(COUNT);
		m_isValidEntity.reserve(COUNT);
	}
	void ECSManager::destroyEntity(const EntityID entityID)
	{
		if (isValid(entityID))
		{
			resetComponentMask(entityID);
			invalidateEntity(entityID);
		}
	}
	void ECSManager::clearEntities()
	{
		m_componentMasks.clear();
		m_componentMasks.shrink_to_fit();
		m_isValidEntity.clear();
		m_isValidEntity.shrink_to_fit();
	}
	
	bool ECSManager::hasInvalidEntities() const noexcept
	{
		return !m_invalidEntityIDs.empty();
	}
	EntityID ECSManager::getAndPopLastInvalidEntityID()
	{
		const EntityID ID = m_invalidEntityIDs.back();
		m_invalidEntityIDs.pop_back();
		return ID;
	}
	EntityID ECSManager::createNewEntity()
	{
		const EntityID entityID = static_cast<EntityID>(m_componentMasks.size());

		m_componentMasks.push_back(0ULL);
		m_isValidEntity.push_back(true);

		return entityID;
	}
	void ECSManager::resetAndValidateEntity(const EntityID entityID)
	{
		resetComponentMask(entityID);
		m_isValidEntity[entityID] = true;
	}
	void ECSManager::resetComponentMask(const EntityID entityID)
	{
		m_componentMasks[entityID] = 0ULL;
	}
	void ECSManager::invalidateEntity(const EntityID entityID)
	{
		m_invalidEntityIDs.push_back(entityID);
		m_isValidEntity[entityID] = false;
	}
}