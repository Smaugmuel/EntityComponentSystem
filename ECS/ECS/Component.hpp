#pragma once

namespace ECS
{
	typedef unsigned int ComponentTypeID;
	extern ComponentTypeID g_component_ID_counter;

	/*
		Do not inherit directly from this class
		See Component below
	*/
	struct BaseComponent
	{
		virtual ~BaseComponent();

		/*
			Do not call this directly
		*/
		static const ComponentTypeID createID()
		{
			return g_component_ID_counter++;
		}

		/*
			Do not call this directly
		*/
		static const ComponentTypeID nrOfComponentTypes()
		{
			return g_component_ID_counter;
		}

		bool valid() const
		{
			return m_valid;
		}
	protected:
		BaseComponent();
		bool m_valid;
	};

	/*
		Components must inherit from this class, i.e. like below
		class MovementComponent : public Component<MovementComponent>
	*/
	template<typename T>
	struct Component : public BaseComponent
	{
		virtual ~Component() {}
		static const ComponentTypeID ID;
	protected:
		Component() {}
	};

	/*
		Assigns dynamically during compilation
	*/
	template<typename T>
	const ComponentTypeID Component<T>::ID = BaseComponent::createID();
}