#include "Utility.hpp"

std::string Utils::prepend(const std::string& original, const size_t N, const char C)
{
	return std::string(N, C) + original;
}

std::string Utils::prependToLength(const std::string& original, const size_t N, const char C)
{
	return prepend(original, N - original.size(), C);
}
