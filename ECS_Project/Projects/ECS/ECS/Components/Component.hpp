#pragma once

namespace ECS
{
	using ComponentTypeID = unsigned char;
	using EntityID = int;

	/*
		Do NOT create and later delete a pointer to this struct and assume its child class' destructors will be called.

		As the inclusion of any virtual function results in a v-table being created,
		which increases the size by 8 Bytes (a pointer to the v-table),
		the virtual destructor is omitted.

		This struct only exists in order to simplify creation of component types,
		while still guaranteeing they have the type ID and entity ID members.		
	*/
	template<ComponentTypeID ID>
	struct Component
	{
		static constexpr ComponentTypeID TYPE_ID = ID;
		//EntityID entityID;
	};

}

// Used as a singleton component tag
#define MAKE_SINGLETON static constexpr bool IS_SINGLETON = true