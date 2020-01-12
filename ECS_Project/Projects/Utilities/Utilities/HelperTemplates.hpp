#pragma once
#include <type_traits>
#include <tuple>

/*
	Default types
*/

// A representation of several data types without any stored data
template<typename... T> struct TypeList {};


/*
	Bool expressions
*/

// Evaluates to true if type T is the same type as at least one of Ts
template<typename T, typename... Ts>
struct is_any_of
{
	static constexpr bool value = static_cast<bool>(std::disjunction<std::is_same<T, Ts>...>::value);
};

// Abbrevieated value
template<typename T, typename... Ts>
inline constexpr bool is_any_of_v = is_any_of<T, Ts...>::value;

// Default type to allow TypeListing below
template<typename... T> struct has_any_common {};

// Evaluates to true if any of the types T are found in the types R
template<typename... T, typename... R>
struct has_any_common<TypeList<T...>, TypeList<R...>>
{
	static constexpr bool value = (is_any_of_v<T, R...> || ...);
};


/*
	Type enablers
*/

// Evaluates to type R if type T is the same type as at least one of Ts
template<typename T, typename R, typename... Ts>
using enable_R_if_any_of = std::enable_if<is_any_of_v<T, Ts...>, R>;

// Evaluates to type R if type T is the same type as at least one of Ts, and extracts the type
template<typename T, typename R, typename... Ts>
using enable_R_if_any_of_t = typename enable_R_if_any_of<T, R, Ts...>::type;

// Evaluates to type T if it's the same type as at least one of Ts
template<typename T, typename... Ts>
using enable_if_any_of = enable_R_if_any_of<T, T, Ts...>;

// Evaluates to type T if it's the same type as at least one of Ts, and extracts the type
template<typename T, typename... Ts>
using enable_if_any_of_t = typename enable_if_any_of<T, Ts...>::type;


/*
	Type mapping
*/

// Maps an int to a type
template<int index, typename... Types>
struct int_to_type { using type = typename std::tuple_element<index, std::tuple<Types...>>::type; };