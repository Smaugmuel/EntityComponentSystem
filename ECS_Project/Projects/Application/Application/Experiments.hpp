#pragma once

#include <tuple>
#include <type_traits>
#include "Utility/HelperTemplates.hpp"

#pragma region Disjunct
// std::disjunction_v<Traits...> retrieves the value of the first Traits whose value member is true
// If none are true, the value of the last Traits is retrieved instead
static constexpr auto DISJUNCT_A = std::disjunction<std::is_same<int, float>, std::integral_constant<int, 543>>::value;
static constexpr auto DISJUNCT_B = std::disjunction<std::is_same<int, float>, std::integral_constant<int, 0>>::value;
static constexpr auto DISJUNCT_C = std::disjunction<std::is_same<int, int>, std::integral_constant<int, 543>>::value;
static constexpr auto DISJUNCT_D = std::disjunction<std::is_same<int, int>, std::integral_constant<int, 0>>::value;
static constexpr auto DISJUNCT_E = std::disjunction<std::integral_constant<int, 543>, std::is_same<int, float>>::value;
static constexpr auto DISJUNCT_F = std::disjunction<std::integral_constant<int, 0>, std::is_same<int, float>>::value;
static constexpr auto DISJUNCT_G = std::disjunction<std::integral_constant<int, 543>, std::is_same<int, int>>::value;
static constexpr auto DISJUNCT_H = std::disjunction<std::integral_constant<int, 0>, std::is_same<int, int>>::value;
#pragma endregion

#pragma region Components
struct BaseComp
{
	typedef int id_type;
};
struct Pos : public BaseComp
{
	id_type id;
	int x;
	int y;
};
struct Mov : public BaseComp
{
	float mx;
	float my;
};

struct Sys
{
	virtual void update([[maybe_unused]] float dt) = 0;
};

struct MovSys : public Sys
{
	void update([[maybe_unused]] float dt) override
	{

	}
};
struct PosSys : public Sys
{
	void update([[maybe_unused]] float dt) override
	{

	}
};

#pragma endregion

#pragma region Templates
template<typename...>
using try_to_instantiate = void;

template<typename T, typename Attempt = void>
struct is_incrementable : public std::false_type { };

template<class T>
struct is_incrementable<T, try_to_instantiate<decltype(++std::declval<T&>())>> : public std::true_type { };


template<typename, typename = void>
struct has_x : public std::false_type { };

template<typename T>
struct has_x<T, std::void_t<decltype(T::x)>> : public std::true_type { };


template<typename T, typename Attempt = void>
struct has_update : public std::false_type { };

template<typename T>
struct has_update<T, std::void_t<decltype(T::update)>> : public std::true_type { };

template<typename T, typename Attempt = void>
struct has_id : public std::false_type { };

template<typename T>
struct has_id<T, std::void_t<decltype(T::id)>> : public std::true_type { };


template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, float>::type cast_to_float(const T& value)
{
	return static_cast<float>(value);
}

template<typename T>
typename std::enable_if<std::is_base_of<BaseComp, T>::value && has_id<T>::value, BaseComp::id_type>::type get_id(const T& comp)
{
	return comp.id;
}


template<unsigned int N>
struct factorial
{
	static constexpr unsigned int value = N * factorial<N - 1>::value;
	enum { enumValue = N * factorial<N - 1>::enumValue };
};
template<>
struct factorial<0>
{
	static constexpr unsigned int value = 1;
	enum { enumValue = 1 };
};


template<typename T, typename... TS>
constexpr typename std::enable_if_t<static_cast<bool>(std::conjunction<std::is_same<T, TS>...>::value), int> enable_if_all_same([[maybe_unused]] T t, [[maybe_unused]] TS... ts)
{
	return sizeof...(TS) + 1;
}

#pragma endregion

void checkThings()
{
	is_incrementable<int>::value;
	is_incrementable<float>::value;
	is_incrementable<const char*>::value;
	is_incrementable<std::tuple<int, float>>::value;

	has_x<Pos>::value;
	has_x<Mov>::value;

	has_update<MovSys>::value;
	has_update<PosSys>::value;

	factorial<5>::value;
	factorial<5>::enumValue;

	cast_to_float(double(0));

	[[maybe_unused]] Pos p;
	[[maybe_unused]] Mov m;

	//BaseComp::id_type id1 = get_id(p);
	//BaseComp::id_type id2 = get_id(m);

	constexpr int N_ARGS = enable_if_all_same(4, 3, 54);

	int_to_type<4, int, float, double, char, short>::T;


}

void testSwitch()
{
	int number = 2;

	switch (number)
	{
	case 1:
		number += 2;
		break;
	case 2:
		number += 2;
		[[fallthrough]];		// Removes warning of no break. Causes warning if break is written below
	case 3:
		number += 3;
		break;
	default:
		break;
	}
}
