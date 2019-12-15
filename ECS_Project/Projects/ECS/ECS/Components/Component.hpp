#pragma once

namespace ECS
{
	using ComponentTypeID = unsigned int;

	/*
		Do not inherit directly from this class
		See Component below
	*/
	struct BaseComponent
	{
		virtual ~BaseComponent() = default;

		/*
			Do not call this directly
		*/
		static const ComponentTypeID createID()
		{
			return s_component_ID_counter++;
		}

		/*
			Do not call this directly
		*/
		static const ComponentTypeID nrOfComponentTypes()
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

	private:
		inline static ComponentTypeID s_component_ID_counter = 0;
	};

	/*
		Components must inherit from this class, i.e. like
		class MovementComponent : public Component<MovementComponent>
	*/
	template<typename T>
	struct Component : public BaseComponent
	{
		virtual ~Component() = default;
		static const ComponentTypeID ID;
	protected:
		Component() = default;
	};

	/*
		Assigns dynamically during compilation
	*/
	template<typename T>
	const ComponentTypeID Component<T>::ID = BaseComponent::createID();
}