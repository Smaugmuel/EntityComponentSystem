#include "ECS/Components/Component.hpp"

struct Position : public ECS::Component<Position>
{
	float x, y;
};
struct Movement : public ECS::Component<Movement>
{
	float x, y;
};

int main()
{
	
	auto pid = Position::ID;
	auto mid = Movement::ID;

	return 0;
}