#pragma once
#include <vector>

namespace ECS
{
	typedef unsigned int SystemTypeID;
	extern SystemTypeID g_system_ID_counter;

	/*
		Do not inherit directly from this class
		See System below
	*/
	class BaseSystem
	{
	public:
		virtual ~BaseSystem();

		/*
			Used internally to assign IDs. Do not call this
		*/
		static SystemTypeID createID()
		{
			return g_system_ID_counter++;
		}
		static SystemTypeID nrOfSystems()
		{
			return g_system_ID_counter;
		}
	protected:
		BaseSystem();
	};

	/*
		Systems must inherit from this class, i.e. like below
		class MovementSystem : public System<MovementSystem>
	*/
	template<typename T>
	class System : public BaseSystem
	{
	public:
		virtual ~System() {}

		static const SystemTypeID ID;
	protected:
		System() {}
	};

	/*
		Assigns ID's dynamically during compilation
	*/
	template<typename T>
	const SystemTypeID System<T>::ID = BaseSystem::createID();


	/*
		Might not need a SystemManager, if the developers themselves manage the systems
	*/
}