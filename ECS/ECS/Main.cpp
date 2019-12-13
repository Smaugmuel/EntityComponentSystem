//#include "ECS.hpp"
#include <iostream>
#include <crtdbg.h>
#include <Windows.h>
#include <string>
#include <sstream>

#include "ECSManager.hpp"
#include "ComponentPool.hpp"
#include "ComponentView.hpp"
#include "Systems/ComponentSystems.hpp"
#include "Components/Components.hpp"

#include "SparseSetBenchmark.hpp"
#include "Timer.hpp"
#include "Utility.hpp"

void printStats()
{

}
void createEntities(ECS::ECSManager& em, const size_t COUNT)
{
	for (size_t i = 0; i < COUNT; i++)
	{
		auto entity = em.createEntity();

		if (i % 1 == 0)
		{
			em.attachComponent<Position>(entity, 1.0f, 3.4f);
		}
		if (i % 2 == 0)
		{
			em.attachComponent<Movement>(entity, 3.0f, 1.4f);
		}
		if (i % 3 == 0)
		{
			em.attachComponent<Acceleration>(entity, 13.0f, 33.5f);
		}
	}
}
void addAndMeasure(ECS::ECSManager& em)
{
	{
		Timer::Scoped timer("Pos, !Acc:");
		int count = 0;
		auto view = em.getView<Position>(TypeList<Acceleration>());
		view.for_each_entity([&count](Position& pos)
		{
			pos.x += 1;
			pos.y += 3;
			count++;
		});

		timer.label += std::to_string(count) + "\t";
	}
	{
		Timer::Scoped timer("Pos      :");
		int count = 0;
		auto view = em.getView<Position>();
		view.for_each_entity([&count](Position& pos)
		{
			pos.x += 1;
			pos.y += 43;
			count++;
		});

		timer.label += std::to_string(count) + "\t";
	}
	{
		Timer::Scoped timer("Pos,  Acc:");
		int count = 0;
		auto view = em.getView<Position, Acceleration>();
		view.for_each_entity([&count](Position& pos, Acceleration& acc)
		{
			pos.x += acc.x;
			pos.y += acc.y;
			count++;
		});

		timer.label += std::to_string(count) + "\t";
	}
	{
		Timer::Scoped timer("Acc,  Pos:");
		int count = 0;
		auto view = em.getView<Acceleration, Position>();
		view.for_each_entity([&count](Acceleration& acc, Position& pos)
		{
			pos.x += acc.x;
			pos.y += acc.y;
			count++;
		});

		timer.label += std::to_string(count) + "\t";
	}
	{
		Timer::Scoped timer("      Acc:");
		int count = 0;
		auto view = em.getView<Acceleration>();
		view.for_each_entity([&count](Acceleration& acc)
		{
			acc.x += 1;
			acc.y += 3;
			count++;
		});

		timer.label += std::to_string(count) + "\t";
	}
	{
		Timer::Scoped timer("Acc, !Pos:");
		int count = 0;
		auto view = em.getView<Acceleration>(TypeList<Position>());
		view.for_each_entity([&count](Acceleration& acc)
		{
			acc.x += 1;
			acc.y += 3;
			count++;
		});

		timer.label += std::to_string(count) + "\t\t";
	}
}
void test4()
{
	constexpr size_t COUNT = 100000;
	ECS::ECSManager em;
	
	{
		Timer::Scoped timer("Entity creation");
		//em.reserveEntities(COUNT);	// Does almost nothing for performance
		createEntities(em, COUNT);
	}

	{
		Timer::Scoped timer("External function");
		addAndMeasure(em);
	}

	std::cin.get();
}
void test5()
{
	SparseSetBenchmark<Position> posBenchmark;
	posBenchmark.runBenchMark();
	std::cout << posBenchmark.getResultString() << "Benchmark size: " << posBenchmark.getBenchmarkByteSize() << "\n";

	std::cin.get();
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ECS::ECSManager em;
	{
		Timer::Scoped timer("Creation :");
		createEntities(em, 1'000'000);
		timer.label += "\t\t";
	}
	{
		Timer::Scoped timer("Measuring:");
		addAndMeasure(em);
		timer.label += "\t\t";
	}

	auto& storage = Timer::getStorage();
	for (auto&[first, second] : storage)
	{
		std::cout << first << " " << second << "  \tns = " << second / 1e6 << " ms \n";
	}

	std::cin.get();

	return 0;
}