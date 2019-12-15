#include "ECS/ECSManager.hpp"

struct Position : public ECS::Component<Position>
{
	Position(float _x, float _y) : x(_x), y(_y) {}
	float x, y;
};
struct Movement : public ECS::Component<Movement>
{
	Movement(float _x, float _y) : x(_x), y(_y) {}
	float x, y;
};

int main()
{
	ECS::ECSManager em;
	auto entity = em.createEntity();

	em.attachComponent<Position>(entity, 43.0f, 12.0f);
	em.attachComponent<Movement>(entity, 78.0f, 132.0f);

	auto view = em.getView<Position, Movement>();
	view.for_each_entity(
		[](Position& pos, Movement& mov)
		{
			pos.x += mov.x;
			pos.y += mov.y; 
		}
	);


	auto pid = Position::ID;
	auto mid = Movement::ID;

	return 0;
}