#pragma once

namespace ECS
{
	class BaseEventReceiver
	{
	public:
		virtual ~BaseEventReceiver() = default;
	protected:
		BaseEventReceiver() = default;
	};


	template<typename T>
	class EventReceiver : public BaseEventReceiver
	{
	public:
		virtual ~EventReceiver() = default;
	protected:
		EventReceiver() = default;
	};
}