#pragma once
#include <vector>
#include <type_traits>
#include "Event.hpp"
#include "EventReceiver.hpp"

namespace Events
{
	template<typename ReceiverType>
	using enable_if_receiver = std::enable_if_t<std::is_base_of<EventReceiver<ReceiverType>, ReceiverType>::value, void>;

	class EventManager final
	{
	public:
		EventManager() = default;
		~EventManager() = default;

		template<typename EventType, typename ReceiverType>
		enable_if_receiver<ReceiverType> subscribe(ReceiverType& receiver)
		{
			if (EventType::ID >= m_receivers.size())
			{
				m_receivers.resize(EventType::ID + 1);
			}
			void (ReceiverType::*receive)(const EventType&) = &ReceiverType::receive;
		}

		template<typename EventType>
		void emit(const EventType& e)
		{
			const auto& receivers = m_receivers[EventType::ID];
			for (auto r : receivers)
			{
				r->receive(e);
			}
		}

	private:
		std::vector<std::vector<BaseEventReceiver*>> m_receivers;
	};
}