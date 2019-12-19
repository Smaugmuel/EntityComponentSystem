#pragma once
#include "Entity.h"
#include "Components/Component.hpp"
#include "Components/ComponentPool.hpp"
#include "Components/ComponentView.hpp"
#include "Utilities/HelperTemplates.hpp"

namespace ECS
{
	// Evaluates to true if type T is a component
	template<typename T>
	using is_component = std::is_base_of<Component<T>, T>;

	// Evaluates to type R if type T is a component
	template<typename T, typename R>
	using enable_if_component = typename std::enable_if_t<is_component<T>::value, R>;

	// Evaluates to true if type T is a singleton component
	template<typename T>
	using is_singleton_component = std::is_base_of<SingletonComponent, T>;

	// Data type used for the component bitmask
	using Bitmask = size_t;

	class ECSManager final
	{
	public:
#pragma region Interface Declarations
		ECSManager() = default;
		~ECSManager();

		[[nodiscard]] Entity createEntity();
		[[nodiscard]] Bitmask getComponentMask(const EntityID id) const;
		[[nodiscard]] bool isValid(const EntityID ID) const;

		void reserveEntities(const size_t COUNT);
		void destroyEntity(const EntityID entityID);
		void clearEntities();
#pragma endregion

#pragma region Interface Template Declarations
		template<typename... IncludedTypes, typename... ExcludedTypes>
		[[nodiscard]] ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>> getView(TypeList<ExcludedTypes...> = {});

		template<typename CompType>
		[[nodiscard]] bool hasComponent(const Entity& entity) const;
		template<typename CompType>
		[[nodiscard]] bool hasComponent(EntityID entityID) const;

		template<typename CompType, typename... TArgs>
		[[maybe_unused]] CompType* attachComponent(const Entity& entity, const TArgs&... args);
		template<typename CompType, typename... TArgs>
		[[maybe_unused]] CompType* attachComponent(EntityID entityID, const TArgs&... args);

		template<typename CompType>
		void detachComponent(const Entity& entity);

		template<typename CompType>
		[[nodiscard]] size_t sizeOfPool() const;
#pragma endregion

	private:
#pragma region Internal Declarations
		bool hasInvalidEntities() const noexcept;
		EntityID getAndPopLastInvalidEntityID();
		EntityID createNewEntity();
		void resetAndValidateEntity(const EntityID entityID);
		void resetComponentMask(const EntityID entityID);
		void invalidateEntity(const EntityID entityID);
#pragma endregion

#pragma region Internal Template Declarations
		template<typename CompType>
		constexpr ComponentTypeID getID() const noexcept;

		template<typename CompType>
		bool hasPool() const;

		template<typename CompType>
		void createPool();

		template<typename CompType>
		ComponentPool<CompType>* getPool();

		template<typename CompType>
		void addToBitMask(EntityID entityID);

		template<typename CompType>
		void removeFromBitMask(EntityID entityID);
#pragma endregion

	private:
		// Bitwise representation of which components each entity has
		std::vector<Bitmask> m_componentMasks;

		// Validity of each entity
		std::vector<bool> m_isValidEntity;

		// Pools where components are stored
		std::vector<BaseComponentPool*> m_componentPools;

		// Previously created, but later invalidated, entity IDs
		std::vector<EntityID> m_invalidEntityIDs;
	};

#pragma region Interface Template Definitions
	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	[[nodiscard]] inline ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>> ECSManager::getView(TypeList<ExcludedTypes...>)
	{
		static_assert(sizeof...(IncludedTypes) > 0, "ECS::ECSManager::getComponents() : No included types");
		//static_assert(std::disjunction<is_any_of<IncludedTypes, ExcludedTypes...>::value...>::value);		// Make this work somehow	
		return { getPool<IncludedTypes>()..., getPool<ExcludedTypes>()... };
	}

	template<typename CompType>
	[[nodiscard]] inline bool ECSManager::hasComponent(const Entity& entity) const
	{
		static_assert(is_component<CompType>::value);
		return hasComponent<CompType>(entity.ID);
	}

	template<typename CompType>
	inline bool ECSManager::hasComponent(EntityID entityID) const
	{
		static_assert(is_component<CompType>::value);
		// 1 means maximum 32 bit shift. 1ULL means maximum 64 bit shift
		return (m_componentMasks[entityID] & (1ULL << getID<CompType>()));
	}

	template<typename CompType, typename ...TArgs>
	[[maybe_unused]] inline CompType* ECSManager::attachComponent(const Entity& entity, const TArgs& ...args)
	{
		static_assert(is_component<CompType>::value);
		return attachComponent<CompType, TArgs...>(entity.ID, args...);
	}
	
	template<typename CompType, typename ...TArgs>
	[[maybe_unused]] inline CompType* ECSManager::attachComponent(EntityID entityID, const TArgs & ...args)
	{
		static_assert(is_component<CompType>::value);
		if (!isValid(entityID))
		{
			return nullptr;
		}
		if (!hasPool<CompType>())
		{
			createPool<CompType>();
		}

		ComponentPool<CompType>* pool = getPool<CompType>();

		if (!hasComponent<CompType>(entityID))
		{
			pool->components.add(entityID, args...);
			addToBitMask<CompType>(entityID);
		}

		return pool->components.get(entityID);
	}

	template<typename CompType>
	inline void ECSManager::detachComponent(const Entity & entity)
	{
		static_assert(is_component<CompType>::value);
		
		bool canBeDetached = isValid(entity.ID) && hasPool<CompType>() && hasComponent<CompType>(entity);
		if (!canBeDetached)
		{
			return;
		}

		ComponentPool<CompType>* pool = getPool<CompType>();
		pool->components.remove(entity.ID);
		removeFromBitMask<CompType>(entity);
	}

	template<typename CompType>
	inline size_t ECSManager::sizeOfPool() const
	{
		return getPool<CompType>()->components.size();
	}

#pragma endregion

#pragma region Internal Template Definitions
	template<typename CompType>
	inline constexpr ComponentTypeID ECSManager::getID() const noexcept
	{
		static_assert(is_component<CompType>::value);
		return CompType::typeID();
	}

	template<typename CompType>
	inline bool ECSManager::hasPool() const
	{
		static_assert(is_component<CompType>::value);
		const ComponentTypeID compTypeID = getID<CompType>();
		return (compTypeID < m_componentPools.size() && m_componentPools[compTypeID]);
	}

	template<typename CompType>
	inline void ECSManager::createPool()
	{
		static_assert(is_component<CompType>::value);
		const size_t compTypeID = static_cast<const size_t>(getID<CompType>());
		if (compTypeID >= m_componentPools.size())
		{
			m_componentPools.resize(compTypeID + 1, nullptr);
		}
		if (m_componentPools[compTypeID])
		{
			return;
		}
		m_componentPools[compTypeID] = new ComponentPool<CompType>;
	}

	template<typename CompType>
	inline ComponentPool<CompType>* ECSManager::getPool()
	{
		static_assert(is_component<CompType>::value);
		const ComponentTypeID compTypeID = getID<CompType>();
		return static_cast<ComponentPool<CompType>*>(m_componentPools[compTypeID]);
	}

	template<typename CompType>
	inline void ECSManager::addToBitMask(EntityID entityID)
	{
		static_assert(is_component<CompType>::value);
		m_componentMasks[entityID] |= (1ULL << getID<CompType>());
	}

	template<typename CompType>
	inline void ECSManager::removeFromBitMask(EntityID entityID)
	{
		static_assert(is_component<CompType>::value);
		m_componentMasks[entityID] &= ~(1ULL << getID<CompType>());
	}
#pragma endregion
}