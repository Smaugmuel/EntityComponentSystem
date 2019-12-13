#pragma once

namespace ECS
{
	struct BaseEvent
	{
		typedef unsigned int EventTypeID;

		virtual ~BaseEvent() = default;

		static EventTypeID nrOfEventTypes()
		{
			return s_event_id_counter;
		}
	protected:
		BaseEvent() = default;
		static EventTypeID s_event_id_counter;
	};


	template<typename T>
	struct Event : public BaseEvent
	{
		virtual ~Event() = default;
		static EventTypeID type()
		{
			static EventTypeID typeID = s_event_id_counter++;
			return typeID;
		}
	protected:
		Event() = default;
	};
}