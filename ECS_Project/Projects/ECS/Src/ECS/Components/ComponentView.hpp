#pragma once
#include "ComponentPool.hpp"
#include "Utility/HelperTemplates.hpp"

#define SINGLE_COMPONENT_OPTIMIZATION

namespace ECS
{
	typedef int EntityID;

	template<typename... T>
	class ComponentView;

	template<typename... IncludedTypes, typename... ExcludedTypes>
	class ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>  final
	{
		static_assert(sizeof...(IncludedTypes) > 0, "No included types found");

	public:
		ComponentView(ComponentPool<IncludedTypes>*... includedPools, ComponentPool<ExcludedTypes>*... excludedPools);
		ComponentView(const ComponentView& other) = default;
		~ComponentView() = default;
		ComponentView& operator=(const ComponentView& other) = default;

		// Retrieves a pointer to a component of type T which is attached to an entity with the specified ID
		// Needs more work
		template<typename CompType>
		CompType* get(const EntityID entityID);

		template<typename Function>
		void for_each_entity(Function f);
		

	private:
		// Retrieves a pointer to a component pool of type T
		template<typename CompType>
		ComponentPool<CompType>& getPool();

		template<typename Func>
		void iterate(Func f);

		/*auto* getSmallestPool()
		{
			const int size[] = { getPool<IncludedTypes>().components.size()... };
			constexpr size_t nTypes = sizeof...(IncludedTypes);

			int smallestSizeIdx = 0;

			for (size_t i = 1; i < nTypes; i++)
			{
				if (size[i] < size[smallestSizeIdx])
				{
					smallestSizeIdx = i;
				}
			}

			return what;
		}*/

	private:
		// Pointers to any number of component pools of different types
		const std::tuple<ComponentPool<IncludedTypes>*...> m_includedPools;
		const std::tuple<ComponentPool<ExcludedTypes>*...> m_excludedPools;
	};

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	inline ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::ComponentView(ComponentPool<IncludedTypes>* ...includedPools, ComponentPool<ExcludedTypes>* ...excludedPools) :
		m_includedPools{ includedPools... }, m_excludedPools{ excludedPools... }
	{
	}

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename CompType>
	inline CompType * ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::get(const EntityID entityID)
	{
		static_assert(static_cast<bool>(is_any_of<CompType, IncludedTypes...>::value), "CompType is not an included type");
		return getPool<CompType>().components.get(entityID);
	}
	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Function>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::for_each_entity(Function f)
	{
#ifdef SINGLE_COMPONENT_OPTIMIZATION
		if constexpr (sizeof...(IncludedTypes) == 1 && sizeof...(ExcludedTypes) == 0)
		{
			// Iterate components directly without retrieving entity index first
			auto& iteratedPool = *std::get<0>(m_includedPools);
			auto& components = iteratedPool.components.getElements();
			const size_t size = iteratedPool.components.size();

			for (size_t i = 0; i < size; i++)
			{
				f(components[i]);
			}
			return;
		}
#endif
		iterate(f);
	}
	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename CompType>
	inline ComponentPool<CompType>& ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::getPool()
	{
		constexpr bool is_included = static_cast<bool>(is_any_of<CompType, IncludedTypes...>::value);
		constexpr bool is_excluded = static_cast<bool>(is_any_of<CompType, ExcludedTypes...>::value);
		static_assert(static_cast<bool>(is_included || is_excluded), "CompType is not an included or excluded type");

		if constexpr (is_included)
		{
			return *std::get<ComponentPool<CompType>*>(m_includedPools);
		}
		else  // This else must NOT be removed
		{
			return *std::get<ComponentPool<CompType>*>(m_excludedPools);
		}
	}
	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Func>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::iterate(Func f)
	{
		// TODO: Iterate over the pool with the fewest components

		// Iterate over the first pool to retrieve entity indices
		const auto& sparseSet = std::get<0>(m_includedPools)->components;
		const auto& elemToIdx = sparseSet.getElemToIdx();
		const size_t size = sparseSet.size();

		// Check for components on each entity that has at least a component of the first included type
		for (size_t i = 0; i < size; i++)
		{
			const auto idx = elemToIdx[i];

			const bool hasAllIncluded = (getPool<IncludedTypes>().components.has(idx) && ...);
			const bool hasAnyExcluded = (getPool<ExcludedTypes>().components.has(idx) || ...);

			if (hasAllIncluded && !hasAnyExcluded)
			{
				f(*getPool<IncludedTypes>().components.get(idx)...);
			}
		}
	}
}