#pragma once
#include "../Component.hpp"

struct Position : public ECS::Component<Position>
{
	Position(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) { }
	float x;
	float y;
};
struct Movement : public ECS::Component<Movement>
{
	Movement(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) { }
	float x;
	float y;
};
struct Acceleration : public ECS::Component<Acceleration>
{
	Acceleration(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) { }
	float x;
	float y;
};