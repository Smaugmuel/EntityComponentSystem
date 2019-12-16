#pragma once
#include "Utilities/SparseSet.hpp"

namespace ECS
{
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
}