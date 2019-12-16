#pragma once
#include <string>

namespace Utils
{
	std::string prepend(const std::string& original, const size_t N, const char C);
	std::string prependToLength(const std::string& original, const size_t N, const char C);
}