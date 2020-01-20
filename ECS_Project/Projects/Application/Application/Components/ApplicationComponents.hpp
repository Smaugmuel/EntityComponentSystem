#pragma once
#include "ECS/Components/Component.hpp"
#include <iostream>

struct Data
{
	Data()
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
		std::cout << "Data was default constructed\n";
	}
	Data(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {
		std::cout << "Data was specifically constructed\n";
	}
	Data(const Data& other)
		: x(other.x), y(other.y), z(other.z), w(other.w) {
		std::cout << "Data was copy constructed\n";
	}
	Data(Data&& other) noexcept
		: x(other.x), y(other.y), z(other.z), w(other.w) {
		std::cout << "Data was move constructed\n";
	}

	Data& operator=(const Data& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;

		std::cout << "Data was copy assigned\n";
	}
	Data& operator=(Data&& other) {
		x = std::move(other.x);
		y = std::move(other.y);
		z = std::move(other.z);
		w = std::move(other.w);

		std::cout << "Data was move assigned\n";
	}

	~Data() {
		std::cout << "Data was destructed\n";
	}

	float x, y, z, w;
};




struct Position
{
	COMPONENT_ID(0);
	Position(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Movement
{
	COMPONENT_ID(1);
	Movement(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Acceleration
{
	COMPONENT_ID(2);
	Acceleration(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Gravity
{
	COMPONENT_ID(3);
	Gravity(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
	float x, y;
};
struct Input
{
	COMPONENT_ID(4);
	MAKE_SINGLETON;

	static constexpr unsigned short NR_OF_KEYS = 256ui16;

	Input() = default;
	bool keysDown    [NR_OF_KEYS] = { false };
	bool keysDownPrev[NR_OF_KEYS] = { false };
};
struct Commands
{
	COMPONENT_ID(5);
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

	bool isMovingForward  = false;
	bool isMovingBackward = false;
	bool isMovingRight    = false;
	bool isMovingLeft     = false;
	bool isMovingUp       = false;
	bool isMovingDown     = false;
	bool isShooting       = false;
};
struct LifeTime
{
	COMPONENT_ID(6);
	LifeTime(float time = 1.0f) : remainingTime(time) {}
	float remainingTime;
};

struct TestingCopies
{
	COMPONENT_ID(7);
	TestingCopies(const Data& d) : data(d) {}
	Data data;
};