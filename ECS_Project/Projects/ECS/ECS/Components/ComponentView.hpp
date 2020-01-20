#pragma once
#include "ComponentPool.hpp"
#include "Utilities/HelperTemplates.hpp"
#include "ECSTemplates.hpp"

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
			return ((1 << T::TYPE_ID) | ...);
		}
	}

	template<typename... T>
	class ComponentView;

	template<typename... IncludedTypes, typename... ExcludedTypes>
	class ComponentView<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>  final
	{
		// Abbreviation of a type
		using any_common_comps = has_any_common<TypeList<IncludedTypes...>, TypeList<ExcludedTypes...>>;

		static_assert(sizeof...(IncludedTypes) > 0, "No included types found");
		static_assert(!any_common_comps::value, "Included and excluded share a type");


	public:
		ComponentView() = delete;
		ComponentView(ComponentPool<IncludedTypes>*... includedPools, ComponentPool<ExcludedTypes>*... excludedPools) : 
			m_includedPools{ includedPools... }, m_excludedPools{ excludedPools... } {}
		ComponentView(const ComponentView& other) = default;
		~ComponentView() = default;
		ComponentView& operator=(const ComponentView& other) = default;

		// Performs the passed function on each entity with all of the included components and none of the exlcuded ones
		// The included components are sent as reference arguments to the function
		template<typename Function>
		void for_each_entity(Function f)
		{
			(getPool<IncludedTypes>().components.sort(), ...);

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

		// Retrieves a pointer to a component of type T which is attached to an entity with the specified ID
		// TODO: More work
		template<typename CompType>
		CompType* get(const EntityID entityID)
		{
			static_assert(is_any_of_v<CompType, IncludedTypes...>, "CompType is not an included type");

			if constexpr (is_singleton<CompType>::value)
			{
				return getPool<CompType>().components.get(0);
			}
			else
			{
				return getPool<CompType>().components.get(entityID);
			}
		}

	public:
		static constexpr Bitmask INCLUDED_MASK = calculateMask<IncludedTypes...>();
		static constexpr Bitmask EXCLUDED_MASK = calculateMask<ExcludedTypes...>();

	private:
		template<typename CompType>
		ComponentPool<CompType>& getPool()
		{
			constexpr bool is_included = is_any_of_v<CompType, IncludedTypes...>;
			constexpr bool is_excluded = is_any_of_v<CompType, ExcludedTypes...>;
			static_assert(static_cast<bool>(is_included || is_excluded), "CompType is not an included or excluded type");

			if constexpr (is_included)
			{
				return *std::get<ComponentPool<CompType>*>(m_includedPools);
			}
			else // This else MUST remain, due to compile-time checks
			{
				return *std::get<ComponentPool<CompType>*>(m_excludedPools);
			}
		}

		template<typename Func>
		void iterateSingleWithoutExcludes(Func f)
		{
			// Iterate components from the first included pool directly
			auto& components = std::get<0>(m_includedPools)->components.getElements();

			for (auto& comp : components)
			{
				f(comp);
			}
		}
		template<typename Func>
		void iterateSingleWithExcludes(Func f)
		{
			// Iterate components from the first included pool directly and retrieve entity indices to check excluded components
			auto& sparseSet = std::get<0>(m_includedPools)->components;
			auto& components = sparseSet.getElements();
			const auto& elemToIndex = sparseSet.getElemToIndex();
			const size_t size = sparseSet.size();

			for (size_t i = 0; i < size; i++)
			{
				const auto entityIndex = elemToIndex[i];
				const bool hasAnyExcluded = (getPool<ExcludedTypes>().components.has(entityIndex) || ...);
				if (!hasAnyExcluded)
				{
					f(components[i]);
				}
			}
		}
		template<typename Func>
		void iterateMultiWithoutExcludes(Func f)
		{
			// Iterate entity indices from the first included pool and check all included components
			auto& sparseSet = std::get<0>(m_includedPools)->components;
			const auto& elemToIndex = sparseSet.getElemToIndex();
			const size_t size = sparseSet.size();

			for (size_t i = 0; i < size; i++)
			{
				const auto entityIndex = elemToIndex[i];
				const bool hasAllIncluded = (getPool<IncludedTypes>().components.has(entityIndex) || ...);
				if (hasAllIncluded)
				{
					f(*getPool<IncludedTypes>().components.get(entityIndex)...);
				}
			}
		}
		template<typename Func>
		void iterateMultiWithExcludes(Func f)
		{
			// Iterate entity indices from the first included pool and check all included and excluded components
			auto& sparseSet = std::get<0>(m_includedPools)->components;
			const auto& elemToIndex = sparseSet.getElemToIndex();
			const size_t size = sparseSet.size();

			for (size_t i = 0; i < size; i++)
			{
				const auto entityIndex = elemToIndex[i];
				const bool hasAllIncluded = (getPool<IncludedTypes>().components.has(entityIndex) && ...);
				const bool hasAnyExcluded = (getPool<ExcludedTypes>().components.has(entityIndex) || ...);

				const bool hasCorrectComponents = hasAllIncluded && !hasAnyExcluded;

				if (hasCorrectComponents)
				{
					f(*getPool<IncludedTypes>().components.get(entityIndex)...);
				}
			}
		}

	private:
		// Pointers to any number of component pools of different types
		const std::tuple<ComponentPool<IncludedTypes>*...> m_includedPools;
		const std::tuple<ComponentPool<ExcludedTypes>*...> m_excludedPools;
	};
}