#pragma once
#include <type_traits>
#include <tuple>

// A representation of several data types without any stored data
template<typename... T>
struct TypeList {};

// Checks if type T is the same type as at least one of Ts
template<typename T, typename... Ts>
using is_any_of = std::disjunction<std::is_same<T, Ts>...>;

// Evaluates to type T if it's the same type as at least one of Ts
template<typename T, typename... Ts>
using enable_if_any_of = std::enable_if<static_cast<bool>(is_any_of<T, Ts...>::value), T>;

// Evaluates to type T if it's the same type as at least one of Ts, and extracts the type
template<typename T, typename... Ts>
using enable_if_any_of_t = typename enable_if_any_of<T, Ts...>::type;

// Evaluates to type R if type T is the same type as at least one of Ts
template<typename T, typename R, typename... Ts>
using enable_R_if_any_of = std::enable_if<static_cast<bool>(is_any_of<T, Ts...>::value), R>;

// Evaluates to type R if type T is the same type as at least one of Ts, and extracts the type
template<typename T, typename R, typename... Ts>
using enable_R_if_any_of_t = typename enable_R_if_any_of<T, R, Ts...>::type;

// Maps an int to a type
template<int index, typename... Types>
struct int_to_type { using T = typename std::tuple_element<index, std::tuple<Types...>>::type; };