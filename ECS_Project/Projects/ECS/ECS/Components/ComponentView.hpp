#pragma once
#include "ComponentPool.hpp"
#include "Utilities/HelperTemplates.hpp"

namespace ECS
{
	using EntityID = int;
	using Bitmask = size_t;

	template<typename... T>
	static constexpr Bitmask calculateMask()
	{
		if constexpr (sizeof...(T) == 0)
		{
			return 0;
		}
		else
		{
			return ((1 << T::ID) | ...);
		}
	}

	template<typename... T>
	class ComponentView;

	template<typename... IncludedTypes, typename... ExcludedTypes>
	class ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>  final
	{
		static_assert(sizeof...(IncludedTypes) > 0, "No included types found");

	public:
		ComponentView(ComponentPool<IncludedTypes>*... includedPools, ComponentPool<ExcludedTypes>*... excludedPools) : 
			m_includedPools{ includedPools }, m_excludedPools{ excludedPools } {}
		ComponentView(const ComponentView& other) = default;
		~ComponentView() = default;
		ComponentView& operator=(const ComponentView& other) = default;

		// Performs the passed function on each entity with all of the included components and none of the exlcuded ones
		// The included components are sent as reference arguments to the function
		template<typename Function>
		void for_each_entity(Function f);

		// Retrieves a pointer to a component of type T which is attached to an entity with the specified ID
		// TODO: More work
		template<typename CompType>
		CompType* get(const EntityID entityID);		

	public:
		static constexpr Bitmask INCLUDED_MASK = calculateMask<IncludedTypes...>();
		static constexpr Bitmask EXCLUDED_MASK = calculateMask<ExcludedTypes...>();

	private:
		// Retrieves a pointer to a component pool of type T
		template<typename CompType>
		ComponentPool<CompType>& getPool();

		template<typename Func>
		void iterateSingleWithoutExcludes(Func f);
		template<typename Func>
		void iterateSingleWithExcludes(Func f);
		template<typename Func>
		void iterateMultiWithoutExcludes(Func f);
		template<typename Func>
		void iterateMultiWithExcludes(Func f);

	private:
		// Pointers to any number of component pools of different types
		const std::tuple<ComponentPool<IncludedTypes>*...> m_includedPools;
		const std::tuple<ComponentPool<ExcludedTypes>*...> m_excludedPools;
	};
	
	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename CompType>
	inline CompType * ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::get(const EntityID entityID)
	{
		static_assert(static_cast<bool>(is_any_of<CompType, IncludedTypes...>::value), "CompType is not an included type");
		return getPool<CompType>().components.get(entityID);
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
		else  // This else MUST remain, due to compile-time checks
		{
			return *std::get<ComponentPool<CompType>*>(m_excludedPools);
		}
	}


	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Function>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::for_each_entity(Function f)
	{
		if constexpr (sizeof...(IncludedTypes) == 1)
		{
			if constexpr (sizeof...(ExcludedTypes) == 0)
			{
				iterateSingleWithoutExcludes(f);
			}
			else
			{
				iterateSingleWithExcludes(f);
			}
		}
		else
		{
			if constexpr (sizeof...(ExcludedTypes) == 0)
			{
				iterateMultiWithoutExcludes(f);
			}
			else
			{
				iterateMultiWithExcludes(f);
			}
		}
	}

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Func>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::iterateSingleWithoutExcludes(Func f)
	{
		// Iterate components from the first included pool directly
		auto& components = std::get<0>(m_includedPools)->components.getElements();

		for (auto& comp : components)
		{
			f(comp);
		}
	}

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Func>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::iterateSingleWithExcludes(Func f)
	{
		// Iterate components from the first included pool directly and retrieve entity indices to check excluded components
		auto& sparseSet = std::get<0>(m_includedPools)->components;
		auto& components = sparseSet.getElements();
		const auto& elemToIdx = sparseSet.getElemToIdx();
		const size_t size = sparseSet.size();

		for (size_t i = 0; i < size; i++)
		{
			const auto entityIdx = elemToIdx[i];
			const bool hasAnyExcluded = (getPool<ExcludedTypes>().components.has(entityIdx) || ...);
			if (!hasAnyExcluded)
			{
				f(components[i]);
			}
		}
	}

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Func>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::iterateMultiWithoutExcludes(Func f)
	{
		// Iterate entity indices from the first included pool and check all included components
		auto& sparseSet = std::get<0>(m_includedPools)->components;
		const auto& elemToIdx = sparseSet.getElemToIdx();
		const size_t size = sparseSet.size();

		for (size_t i = 0; i < size; i++)
		{
			const auto entityIdx = elemToIdx[i];
			const bool hasAllIncluded = (getPool<IncludedTypes>().components.has(entityIdx) || ...);
			if (hasAllIncluded)
			{
				f(*getPool<IncludedTypes>().components.get(entityIdx)...);
			}
		}
	}

	template<typename ...IncludedTypes, typename ...ExcludedTypes>
	template<typename Func>
	inline void ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>::iterateMultiWithExcludes(Func f)
	{
		// Iterate entity indices from the first included pool and check all included and excluded components
		auto& sparseSet = std::get<0>(m_includedPools)->components;
		const auto& elemToIdx = sparseSet.getElemToIdx();
		const size_t size = sparseSet.size();

		for (size_t i = 0; i < size; i++)
		{
			const auto entityIdx = elemToIdx[i];
			const bool hasAllIncluded = (getPool<IncludedTypes>().components.has(entityIdx) && ...);
			const bool hasAnyExcluded = (getPool<ExcludedTypes>().components.has(entityIdx) || ...);

			const bool hasCorrectComponents = hasAllIncluded && !hasAnyExcluded;

			if (hasCorrectComponents)
			{
				f(*getPool<IncludedTypes>().components.get(entityIdx)...);
			}
		}
	}
}