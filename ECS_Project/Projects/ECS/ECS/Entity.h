#pragma once

namespace ECS
{
	class ECSManager;

	using EntityID = int;

	struct Entity final
	{
		Entity(const Entity& other) = default;
		~Entity() = default;

		const EntityID ID;
	private:
		friend class ECSManager;
		Entity() = default;
		Entity(const EntityID _ID) : ID(_ID) {}
	};
}