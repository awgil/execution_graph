#include "basic_engine.hpp"
#include "input.hpp"
#include "render.hpp"
#include "simulation.hpp"

void BasicEngineFeature::setup(SystemManager& world)
{
	world.add<InputSystem>();
	world.add<SimulationSystem>();
	world.add<RenderSystem>();
}

RTTR_REGISTRATION
{
	FeatureRegistration<BasicEngineFeature>("BasicEngineFeature");
}
