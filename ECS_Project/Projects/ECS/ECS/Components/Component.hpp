#pragma once

namespace ECS
{
	using ComponentTypeID = unsigned char;

	/*
		Do not inherit directly from BaseComponent
		See Component below
	*/
	struct BaseComponent
	{
		virtual ~BaseComponent() = default;
		bool valid() const { return m_valid; }
	protected:
		BaseComponent() : m_valid(true) {}
		bool m_valid;
	};

	/*
		Components must inherit from this struct, i.e. like
		struct Movement : public Component<Movement>
	*/
	template<typename T>
	struct Component : public BaseComponent
	{
		virtual ~Component() = default;
	protected:
		Component() = default;
	};


	/*
		Empty type inherited from when declaring a singleton component
		Prefer to use the MAKE_SINGLETON define instead of inheriting from this manually
	*/
	struct SingletonComponent {	};
}