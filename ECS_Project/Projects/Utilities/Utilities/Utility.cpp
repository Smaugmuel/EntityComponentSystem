#include "pch_Utilities.hpp"
#include "Utility.hpp"

namespace Utils
{
	std::string prepend(const std::string& original, const size_t N, const char C)
	{
		return std::string(N, C) + original;
	}

	std::string prependToLength(const std::string& original, const size_t N, const char C)
	{
		return prepend(original, N - original.size(), C);
	}
}