#pragma once
#include "Entity.h"
#include "Components/Component.hpp"
#include "Components/ComponentPool.hpp"
#include "Components/ComponentView.hpp"
#include "Utilities/HelperTemplates.hpp"
#include "ECSTemplates.hpp"

namespace ECS
{
	// Used to keep track of components of an entity
	using Bitmask = size_t;


	class ECSManager final
	{
	public:
		ECSManager() = default;
		~ECSManager();

		[[nodiscard]] Entity createEntity();
		[[nodiscard]] Bitmask getComponentMask(const EntityID entityID) const;
		[[nodiscard]] bool isValid(const EntityID entityID) const;

		void reserveEntities(const size_t COUNT);
		void destroyEntity(const EntityID entityID);
		void clearEntities();

		template<typename... IncludedTypes, typename... ExcludedTypes>
		[[nodiscard]] ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>> getView(TypeList<ExcludedTypes...> = {})
		{
			static_assert(sizeof...(IncludedTypes) > 0, "No included types");
			static_assert(!has_any_common<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::value, "Included and excluded share a type");

			return { getPool<IncludedTypes>()..., getPool<ExcludedTypes>()... };
		}

		template<typename CompType>
		[[nodiscard]] bool hasComponent(const Entity& entity) const
		{
			static_assert(is_component<CompType>::value, "Not a component");
			return hasComponent<CompType>(entity.ID);
		}
		template<typename CompType>
		[[nodiscard]] bool hasComponent(EntityID entityID) const
		{
			static_assert(is_component<CompType>::value, "Not a component");
			// 1 means maximum 32 bit shift. 1ULL means maximum 64 bit shift
			return (m_componentMasks[entityID] & (1ULL << getID<CompType>()));
		}

		template<typename CompType, typename... Args>
		[[maybe_unused]] CompType* attachComponent(const Entity& entity, Args&&... args)
		{
			static_assert(is_component<CompType>::value, "Not a component");
			return attachComponent<CompType, Args...>(entity.ID, std::forward<Args>(args)...);
		}
		template<typename CompType, typename... Args>
		[[maybe_unused]] CompType* attachComponent(EntityID entityID, Args&&... args)
		{
			static_assert(is_component<CompType>::value, "Not a component");

			if (!isValid(entityID))
			{
				return nullptr;
			}
			if (!hasPool<CompType>())
			{
				createPool<CompType>();
			}

			ComponentPool<CompType>* pool = getPool<CompType>();

			if constexpr (is_singleton<CompType>::value)
			{
				if (pool->components.size() == 0)
				{
					pool->components.add(0, std::forward<Args>(args)...);
				}

				if (!hasComponent<CompType>(entityID))
				{
					addToBitMask<CompType>(entityID);
				}

				return pool->components.get(0);
			}
			else
			{
				if (!hasComponent<CompType>(entityID))
				{
					pool->components.add(entityID, std::forward<Args>(args)...);
					addToBitMask<CompType>(entityID);
				}

				return pool->components.get(entityID);
			}
		}

		template<typename CompType>
		void detachComponent(const Entity& entity)
		{
			static_assert(is_component<CompType>::value, "Not a component");

			bool canBeDetached = isValid(entity.ID) && hasPool<CompType>() && hasComponent<CompType>(entity);
			if (!canBeDetached)
			{
				return;
			}

			ComponentPool<CompType>* pool = getPool<CompType>();
			pool->components.remove(entity.ID);
			removeFromBitMask<CompType>(entity.ID);
		}

		template<typename CompType>
		[[nodiscard]] size_t sizeOfPool() const
		{
			return getPool<CompType>()->components.size();
		}

	private:
		template<typename CompType>
		static constexpr ComponentTypeID getID() noexcept
		{
			static_assert(is_component<CompType>::value, "Not a component");
			return CompType::TYPE_ID;
		}

		template<typename CompType>
		bool hasPool() const
		{
			static_assert(is_component<CompType>::value, "Not a component");
			static constexpr ComponentTypeID compTypeID = getID<CompType>();
			return (compTypeID < m_componentPools.size() && m_componentPools[compTypeID]);
		}

		template<typename CompType>
		void createPool()
		{
			static_assert(is_component<CompType>::value, "Not a component");
			static constexpr size_t compTypeID = static_cast<const size_t>(getID<CompType>());
			if (compTypeID >= m_componentPools.size())
			{
				m_componentPools.resize(compTypeID + 1, nullptr);
			}
			if (m_componentPools[compTypeID])
			{
				return;
			}
			m_componentPools[compTypeID] = new ComponentPool<CompType>;		// TODO: change allocation method
		}

		template<typename CompType>
		ComponentPool<CompType>* getPool()
		{
			static_assert(is_component<CompType>::value, "Not a component");
			static constexpr ComponentTypeID compTypeID = getID<CompType>();
			return static_cast<ComponentPool<CompType>*>(m_componentPools[compTypeID]);
		}

		template<typename CompType>
		void addToBitMask(EntityID entityID)
		{
			static_assert(is_component<CompType>::value, "Not a component");
			m_componentMasks[entityID] |= (1ULL << getID<CompType>());
		}

		template<typename CompType>
		void removeFromBitMask(EntityID entityID)
		{
			static_assert(is_component<CompType>::value, "Not a component");
			m_componentMasks[entityID] &= ~(1ULL << getID<CompType>());
		}

		bool hasInvalidEntities() const noexcept;
		EntityID getAndPopLastInvalidEntityID();
		EntityID createNewEntity();
		void resetAndValidateEntity(const EntityID entityID);
		void resetComponentMask(const EntityID entityID);
		void invalidateEntity(const EntityID entityID);

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
}