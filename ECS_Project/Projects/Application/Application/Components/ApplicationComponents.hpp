#pragma once
#include "ECS/Components/Component.hpp"

struct Position : public ECS::Component<0>
{
	Position(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Movement : public ECS::Component<1>
{
	Movement(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Acceleration : public ECS::Component<2>
{
	Acceleration(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Gravity : public ECS::Component<3>
{
	Gravity(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Input : public ECS::Component<4>
{
	MAKE_SINGLETON;

	static constexpr unsigned short NR_OF_KEYS = 256ui16;

	Input() = default;
	bool keysDown[NR_OF_KEYS] = { false };
	bool keysDownPrev[NR_OF_KEYS] = { false };
};
struct Commands : public ECS::Component<5>
{
	MAKE_SINGLETON;
	Commands() = default;

	void reset()
	{
		isMovingForward = false;
		isMovingBackward = false;
		isMovingRight = false;
		isMovingLeft = false;
		isMovingUp = false;
		isMovingDown = false;
		isShooting = false;
	}

	bool isMovingForward = false;
	bool isMovingBackward = false;
	bool isMovingRight = false;
	bool isMovingLeft = false;
	bool isMovingUp = false;
	bool isMovingDown = false;
	bool isShooting = false;
};
struct LifeTime : public ECS::Component<6>
{
	LifeTime(float time = 1.0f) : remainingTime(time) {}
	float remainingTime;
};