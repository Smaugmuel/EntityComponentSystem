#pragma once
#include "ECS/Components/Component.hpp"

struct Position : public ECS::Component<Position>
{
	static constexpr unsigned char ID = 0ui8;

	Position(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;

};
struct Movement : public ECS::Component<Movement>
{
	static constexpr unsigned char ID = 1ui8;

	Movement(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Acceleration : public ECS::Component<Acceleration>
{
	static constexpr unsigned char ID = 2ui8;

	Acceleration(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Gravity : public ECS::Component<Gravity>
{
	static constexpr unsigned char ID = 3ui8;

	Gravity(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};