#pragma once
#include <type_traits>

namespace ECS
{
	/******************************
	** Component template checks **
	******************************/

	// Default evaluates to false
	template<typename T, typename Attempt = void>
	struct is_component : public std::false_type {};

	// Evaluates to true if type T is a component
	template<typename T>
	struct is_component<T, std::void_t<decltype(T::TYPE_ID)>> : public std::true_type {};

	// Default evaluates to false
	template<typename T, typename Attempt = void>
	struct is_singleton : public std::false_type {};

	// Evaluates to true if type T is singleton
	template<typename T>
	struct is_singleton<T, std::void_t<decltype(T::IS_SINGLETON)>> : public std::true_type {};
}