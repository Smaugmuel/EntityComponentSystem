#pragma once
#include <time.h>
#include "Utilities/Timer.hpp"
#include "ECS/ECSManager.hpp"

namespace Benchmark
{
	template<typename CompType>
	void hasComponents(ECS::ECSManager& em, const size_t count)
	{
		std::srand(static_cast<unsigned int>(time(nullptr)));

		Timer::Scoped timer(std::string("Checking " + std::to_string(count) + " " + typeid(CompType).name()).c_str());
		for (size_t i = 0; i < count; i++)
		{
			em.hasComponent<CompType>(i);
		}
	}

	template<typename CompType>
	void attachComponents(ECS::ECSManager& em, const size_t count)
	{
		std::srand(static_cast<unsigned int>(time(nullptr)));
		
		Timer::Scoped timer(std::string("Attaching " + std::to_string(count) +  " " + typeid(CompType).name()).c_str());
		for (size_t i = 0; i < count; i++)
		{
			em.attachComponent<CompType>(i);
		}
	}
}