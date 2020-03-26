#include "input.hpp"

void InputSystem::execute(tf::Subflow&, float dt)
{
	printf("Input; %f passed since prev frame\n", dt);
}

RTTR_REGISTRATION
{
	FunctorRegistration<InputSystem>("InputSystem");
}
