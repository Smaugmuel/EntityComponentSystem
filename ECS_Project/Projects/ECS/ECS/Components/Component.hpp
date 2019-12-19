#pragma once

namespace ECS
{
	using ComponentTypeID = unsigned int;

	/*
	
		NOTE:

		Component type IDs will assign automatically when calling type() for the first time.
		If a certain type never calls type(), the assign will never happen.
		This means that nrOfComponentTypes() will return the number of types actually in use,
		not the number of declared types.

	*/


	/*
		Do not inherit directly from BaseComponent
		See Component below
	*/
	struct BaseComponent
	{
		virtual ~BaseComponent() = default;

		// Returns the number of component types in use
		// (MIGHT CHANGE SOON)
		static ComponentTypeID nrOfComponentTypes()
		{
			return s_component_ID_counter;
		}

		bool valid() const
		{
			return m_valid;
		}
	protected:
		BaseComponent() : m_valid(true) {}
		bool m_valid;

		// Do not modify this manually
		inline static ComponentTypeID s_component_ID_counter = 0;
	};

	/*
		Components must inherit from this struct, i.e. like
		struct Movement : public Component<Movement>
	*/
	template<typename T>
	struct Component : public BaseComponent
	{
		virtual ~Component() = default;

		// Returns this type's identifier ID
		static ComponentTypeID typeID()
		{
			static ComponentTypeID ID = s_component_ID_counter++;
			return ID;
		}
	protected:
		Component() = default;
	};


	/*
		Empty type used only when making and checking if a component is a singleton
	*/
	struct SingletonComponent { };
}