#include "input.hpp"

void InputSystem::operator()(float dt)
{
	printf("Input; %f passed since prev frame\n", dt);
}

// TODO: RTTR registration, edges defined here...
