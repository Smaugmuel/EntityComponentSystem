#pragma once

namespace ECS
{
	using ComponentTypeID = unsigned char;
}

// Used to never make ID an invalid type
#define COMPONENT_ID(x) static constexpr ECS::ComponentTypeID ID = x

// Used as a singleton component tag
#define MAKE_SINGLETON static constexpr bool IS_SINGLETON = true