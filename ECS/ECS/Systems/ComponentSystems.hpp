#pragma once
#include "../System.hpp"
#include "../ECSManager.hpp"
#include "../Components/Components.hpp"

struct MovementSystem final : public ECS::System<MovementSystem>
{
	MovementSystem() = default;
	~MovementSystem() = default;

	void update([[maybe_unused]] ECS::ECSManager& ecs, [[maybe_unused]] const float dt)
	{
		auto view = ecs.getView<Movement, Position>();

		view.for_each_entity([dt](Movement& mov, Position& pos)
		{
			pos.x += mov.x * dt;
			pos.y += mov.y * dt;
		});
	}
};

struct AccelerationSystem final : public ECS::System<AccelerationSystem>
{
	AccelerationSystem() = default;
	~AccelerationSystem() = default;

	void update([[maybe_unused]] ECS::ECSManager& ecs, [[maybe_unused]] const float dt)
	{
		auto view = ecs.getView<Acceleration, Movement>();

		view.for_each_entity([dt](Acceleration& acc, Movement& mov)
		{
			mov.x += acc.x * dt;
			mov.y += acc.y * dt;
		});
	}
};