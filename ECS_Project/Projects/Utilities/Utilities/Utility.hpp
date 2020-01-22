#pragma once
#include <string>

namespace Utils
{
	std::string prepend(const std::string& original, const size_t N, const char C);
	std::string prependToLength(const std::string& original, const size_t N, const char C);

	// Uses move semantics to reduce copies
	template<typename T>
	void swap(T& lhs, T& rhs)
	{
		T temp = std::move(lhs);
		lhs = std::move(rhs);
		rhs = std::move(temp);
	}
}