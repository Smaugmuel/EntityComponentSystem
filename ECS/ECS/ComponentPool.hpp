#pragma once

#define NEW_POOL

#ifdef NEW_POOL
#include "SparseSet.hpp"
#else
#include <vector>
#endif

namespace ECS
{
#ifndef NEW_POOL
	typedef int ComponentID;
	typedef int EntityID;

	class BaseComponentPool
	{
	public:
		virtual ~BaseComponentPool();
		void* getComponentSafe(const EntityID entityID);
		void* getComponentFast(const EntityID entityID);
		const bool hasComponentSafe(const EntityID entityID) const;
		const bool hasComponentFast(const EntityID entityID) const;
		virtual void destroyComponentSafe(const EntityID entityID) = 0;
		virtual void destroyComponentFast(const EntityID entityID) = 0;
		virtual void destroyAllComponents() = 0;
		const ComponentID entityToComponent(const EntityID entityID) const;
		const EntityID componentToEntity(const ComponentID componentID) const;

	protected:
		BaseComponentPool(const ComponentID elementSize);
		void invalidateComponentSafe(const EntityID entityID);
		void invalidateComponentFast(const EntityID entityID);

	protected:
		char* m_componentData;
		const size_t m_elementSize;
		unsigned short m_size;
		unsigned short m_capacity;

		std::vector<ComponentID> m_entityToComponent;
		std::vector<EntityID> m_componentToEntity;
		std::vector<short> m_invalidComponentIDs;

	private:
		void linkEntityAndComponent(const EntityID entityID, const ComponentID componentID);
		void removeLinkSafe(const EntityID entityID, const ComponentID componentID);
		void removeLinkFast(const EntityID entityID, const ComponentID componentID);
		void expand();
		const bool needsExpand() const noexcept;
		const bool poolHasInvalidComponents() const noexcept;
		const ComponentID getAndPopLastInvalidComponentID();
	};

	template<typename T>
	class ComponentPool final : public BaseComponentPool
	{
	public:
		struct Components
		{
			const int count = 0;
			T* data = nullptr;
		};
	
		ComponentPool() : BaseComponentPool::BaseComponentPool(sizeof(T))
		{
		}
		~ComponentPool()
		{
			destroyAllComponents();
		}
	
		void destroyAllComponents() override
		{
			T* startAddress = static_cast<T*>(static_cast<void*>(m_componentData));
			for (unsigned short i = 0; i < m_size; i++)
			{
				T* address = startAddress + i;	// Type will ensure the pointer is moved 1 element, not 1 byte, per iteration
				address->~T();
			}
		}
	
		Components getComponents() const
		{
			return { m_size, static_cast<T*>(static_cast<void*>(m_componentData)) };
		}
	
	private:
		void destroyComponentSafe(const EntityID entityID) override
		{
			if (!hasComponentSafe(entityID))
			{
				return;
			}
	
			destroyComponentFast(entityID);
			invalidateComponentFast(entityID);
		}
		void destroyComponentFast(const EntityID entityID) override
		{
			T* component = static_cast<T*>(getComponentFast(entityID));
			component->~T();
		}
	};

#else
	class BaseComponentPool
	{
	public:
		BaseComponentPool(const BaseComponentPool& other) = delete;
		virtual ~BaseComponentPool() = default;
		BaseComponentPool& operator=(const BaseComponentPool& other) = delete;
	protected:
		BaseComponentPool() = default;
	};

	template<typename T>
	class ComponentPool final : public BaseComponentPool
	{
	public:
		ComponentPool() = default;
		ComponentPool(const ComponentPool& other) = delete;
		~ComponentPool() = default;
		ComponentPool& operator=(const ComponentPool& other) = delete;
		
	public:
		SparseSet<T> components;
	};

#endif
}