#include "pch.h"
#include "ComponentPool.hpp"

namespace ECS
{
#ifndef NEW_POOL
	BaseComponentPool::BaseComponentPool(const ComponentID elementSize) : m_componentData(nullptr), m_elementSize(elementSize), m_size(0), m_capacity(0)
	{
	}
	BaseComponentPool::~BaseComponentPool()
	{
		delete[] m_componentData;
	}

	void* BaseComponentPool::getComponentSafe(const EntityID entityID)
	{
		if (entityID < 0)
		{
			return nullptr;
		}
		if (entityID >= m_entityToComponent.size())
		{
			m_entityToComponent.resize(entityID + 1, -1);
		}
		if (hasComponentFast(entityID))
		{
			return getComponentFast(entityID);
		}
		if (poolHasInvalidComponents())
		{
			const ComponentID availableID = getAndPopLastInvalidComponentID();
			linkEntityAndComponent(entityID, availableID);
			return getComponentFast(entityID);
		}
		if (needsExpand())
		{
			expand();
		}

		linkEntityAndComponent(entityID, static_cast<ComponentID>(m_size++));
		return getComponentFast(entityID);
	}
	void* BaseComponentPool::getComponentFast(const EntityID entityID)
	{
		return m_componentData + m_entityToComponent[entityID] * m_elementSize;
	}
	const bool BaseComponentPool::hasComponentSafe(const EntityID entityID) const
	{
		if (entityID < 0 || entityID >= m_entityToComponent.size())
		{
			return false;
		}
		return hasComponentFast(entityID);
	}
	const bool BaseComponentPool::hasComponentFast(const EntityID entityID) const
	{
		return m_entityToComponent[entityID] != -1;
	}

	const ComponentID BaseComponentPool::entityToComponent(const EntityID entityID) const
	{
		return m_entityToComponent[entityID];
	}
	const EntityID BaseComponentPool::componentToEntity(const ComponentID componentID) const
	{
		return m_componentToEntity[componentID];
	}
	
	void BaseComponentPool::invalidateComponentSafe(const EntityID entityID)
	{
		if (entityID < 0 || entityID >= m_entityToComponent.size())
		{
			return;
		}

		const ComponentID componentID = m_entityToComponent[entityID];
		if (componentID < 0 || componentID >= m_componentToEntity.size())
		{
			return;
		}

		invalidateComponentFast(entityID);
	}
	void BaseComponentPool::invalidateComponentFast(const EntityID entityID)
	{
		const ComponentID componentID = m_entityToComponent[entityID];
		removeLinkFast(entityID, componentID);
		m_invalidComponentIDs.push_back(componentID);
	}

	void BaseComponentPool::linkEntityAndComponent(const EntityID entityID, const ComponentID componentID)
	{
		m_entityToComponent[entityID] = componentID;
		m_componentToEntity[componentID] = entityID;
	}
	void BaseComponentPool::removeLinkSafe(const EntityID entityID, const ComponentID componentID)
	{
		if (entityID < 0 || entityID >= m_entityToComponent.size())
		{
			return;
		}
		if (componentID < 0 || componentID >= m_componentToEntity.size())
		{
			return;
		}
		removeLinkFast(entityID, componentID);
	}
	void BaseComponentPool::removeLinkFast(const EntityID entityID, const ComponentID componentID)
	{
		m_componentToEntity[componentID] = -1;
		m_entityToComponent[entityID] = -1;
	}
	void BaseComponentPool::expand()
	{
		m_capacity += 32;
		char* newData = new char[m_capacity * m_elementSize];
		memcpy(newData, m_componentData, m_size * m_elementSize);
		delete[] m_componentData;
		m_componentData = newData;

		m_componentToEntity.resize(m_capacity, -1);
	}
	const bool BaseComponentPool::needsExpand() const noexcept
	{
		return m_size >= m_capacity;
	}
	const bool BaseComponentPool::poolHasInvalidComponents() const noexcept
	{
		return !m_invalidComponentIDs.empty();
	}
	const ComponentID BaseComponentPool::getAndPopLastInvalidComponentID()
	{
		const ComponentID ID = m_invalidComponentIDs.back();
		m_invalidComponentIDs.pop_back();
		return ID;
	}
#endif
}