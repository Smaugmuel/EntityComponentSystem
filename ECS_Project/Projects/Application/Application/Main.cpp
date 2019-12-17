#include "ECS/ECSManager.hpp"
#include "Components/Components.hpp"
#include "Utilities/Timer.hpp"
#include <iostream>

void createCharacter(ECS::ECSManager& em)
{
	auto entity = em.createEntity();
	em.attachComponent<Position>(entity);
	em.attachComponent<Movement>(entity);
	em.attachComponent<Acceleration>(entity);
	em.attachComponent<Gravity>(entity);
}

void movementSystem(ECS::ECSManager& em, float dt)
{
	auto view = em.getView<Movement, Position>();
	view.for_each_entity([dt](Movement& mov, Position& pos)
		{
			pos.x += mov.x * dt;
			pos.y += mov.y * dt;
		}
	);
}
void accelerationSystem(ECS::ECSManager& em, float dt)
{
	auto view = em.getView<Acceleration, Movement>();
	view.for_each_entity([dt](Acceleration& acc, Movement& mov)
		{
			mov.x += acc.x * dt;
			mov.y += acc.y * dt;
		}
	);
}
void gravitySystem(ECS::ECSManager& em, float dt)
{
	auto view = em.getView<Gravity, Movement>();
	view.for_each_entity([dt](Gravity& grav, Movement& mov)
		{
			mov.x += grav.x * dt;
			mov.y += grav.y * dt;
		}
	);
}

int main()
{
	ECS::ECSManager em;
	Timer::TimePoint tp1, tp2;

	for (size_t i = 0; i < 10'000; i++)
	{
		createCharacter(em);
	}

	constexpr float nsToS = 1.0f / static_cast<float>(1e9);


	ECS::ComponentView<TypeList<Movement, Acceleration>, TypeList<>>::INCLUDED_MASK;
	ECS::ComponentView<TypeList<Movement, Acceleration>, TypeList<Gravity, Position>>::INCLUDED_MASK;
	ECS::ComponentView<TypeList<Movement, Acceleration>, TypeList<Gravity, Position>>::EXCLUDED_MASK;
	ECS::ComponentView<TypeList<Movement, Acceleration>, TypeList<>>::EXCLUDED_MASK;


	Timer::Stopped timer("10'000 iterations");
	for (size_t i = 0; i < 10'000; i++)
	{
		tp1 = tp2;
		tp2 = Timer::Clock::now();
		const float dt = (tp2 - tp1).count() * nsToS;

		accelerationSystem(em, dt);
		gravitySystem(em, dt);
		movementSystem(em, dt);
	}
	timer.stop();

	for (auto& [label, time] : Timer::getStorage())
	{
		std::cout << label << ": " << static_cast<double>(time) / 1e6 << "ms\n";
	}

	return 0;
}