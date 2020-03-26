#pragma once

#include "system_manager.hpp"

// this is an example of a simple system
class InputSystem : public ISystem
{
public:
	using ISystem::ISystem;

	void execute(tf::Subflow& sf, float dt) override;
};
