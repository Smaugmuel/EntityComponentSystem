#pragma once

namespace ECS
{
	/*********************
	** Type definitions **
	*********************/

	using ComponentTypeID = unsigned char;
	using Bitmask = size_t;

	/******************************
	** Component template checks **
	******************************/

	// Default evaluates to false
	template<typename T, typename Attempt = void>
	struct is_component : public std::false_type {};

	// Evaluates to true if type T is a component
	template<typename T>
	struct is_component<T, std::void_t<decltype(T::ID)>> : public std::true_type {};

	// Default evaluates to false
	template<typename T, typename Attempt = void>
	struct is_singleton : public std::false_type {};

	// Evaluates to true if type T is singleton
	template<typename T>
	struct is_singleton<T, std::void_t<decltype(T::IS_SINGLETON)>> : public std::true_type {};
}

// Used to never make ID an invalid type
#define COMPONENT_ID(x) static constexpr ECS::ComponentTypeID ID = x

// Used as a singleton component tag
#define MAKE_SINGLETON static constexpr bool IS_SINGLETON = true