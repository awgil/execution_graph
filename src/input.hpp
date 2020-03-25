#pragma once

#include "system_manager.hpp"

// this is an example of a simple system
class InputSystem : public ISystem
{
public:
	using ISystem::ISystem;

	void operator()(float dt);
};
