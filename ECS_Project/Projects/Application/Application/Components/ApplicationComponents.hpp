#pragma once
#include "ECS/Components/Component.hpp"

#define COMPONENT_TYPE(x) struct x : public ECS::Component<x>
#define COMPONENT_ID(x) static constexpr unsigned char ID = x
#define MAKE_SINGLETON , public ECS::SingletonComponent
#define COMMAND(x) bool x = false

COMPONENT_TYPE(Position)
{
	COMPONENT_ID(0);
	Position(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
COMPONENT_TYPE(Movement)
{
	COMPONENT_ID(1);
	Movement(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
COMPONENT_TYPE(Acceleration)
{
	COMPONENT_ID(2);
	Acceleration(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
COMPONENT_TYPE(Gravity)
{
	COMPONENT_ID(3);
	Gravity(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
COMPONENT_TYPE(Input) MAKE_SINGLETON
{
	COMPONENT_ID(4);

	static constexpr unsigned short NR_OF_KEYS = 256ui16;

	Input() = default;
	bool keysDown    [NR_OF_KEYS] = { false };
	bool keysDownPrev[NR_OF_KEYS] = { false };
};
COMPONENT_TYPE(Commands) MAKE_SINGLETON
{
	COMPONENT_ID(5);

	Commands() = default;

	COMMAND(isWalkingForward);
	COMMAND(isWalkingBackward);
	COMMAND(isWalkingRight);
	COMMAND(isWalkingLeft);
	COMMAND(isJumping);
	COMMAND(isShooting);
};