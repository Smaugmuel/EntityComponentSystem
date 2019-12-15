#pragma once

namespace Events
{
	using EventTypeID = unsigned int;

	/*

		NOTE:

		Event type IDs will assign automatically when calling type() for the first time.
		If a certain type never calls type(), the assign will never happen.
		This means that nrOfEventTypes() will return the number of types actually in use,
		not the number of declared types.

	*/


	/*
		Do not inherit directly from BaseEvent
		See Event below
	*/
	struct BaseEvent
	{
		virtual ~BaseEvent() = default;

		static EventTypeID nrOfEventTypes()
		{
			return s_event_ID_counter;
		}
	protected:
		BaseEvent() = default;

		// Do not modify this manually
		inline static EventTypeID s_event_ID_counter = 0;
	};

	/*
		Events must inherit from this struct, i.e. like
		struct SpawnCharacter : public Event<SpawnCharacter>
	*/
	template<typename T>
	struct Event : public BaseEvent
	{
		virtual ~Event() = default;

		// Returns this type's identifier ID
		static EventTypeID typeID()
		{
			static EventTypeID ID = s_event_ID_counter++;
			return ID;
		}
	protected:
		Event() = default;
	};
}