#pragma once

#include "system_manager.hpp"

// this is an example of a simple system
class InputSystem : public System<InputSystem>
{
public:
	using System::System;

	void execute(tf::Subflow& sf, float dt) override;
};
