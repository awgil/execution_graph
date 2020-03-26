#include "input.hpp"
#include "simulation.hpp"
#include "render.hpp"
#include "model.hpp"

struct SimA : public ISimulationSubsystem
{
	void simulateFrame() override
	{
		printf("Simulation A\n");
	}

	RTTR_ENABLE(ISimulationSubsystem);
};

struct SimB : public ISimulationSubsystem
{
	void simulateFrame() override
	{
		printf("Simulation B\n");
	}

	RTTR_ENABLE(ISimulationSubsystem);
};

struct CustomRenderer : public IBatcherSubsystem
{
	void prepBatch(BatchRenderer& batcher) override
	{
		batcher.addBatch(*this, 3, 30);
	}

	void evalBatch(BatchIter begin, BatchIter end) override
	{
		printf("Rendering custom:\n");
		for (auto i = begin; i != end; ++i)
			printf("- element %d\n", i->data);
	}
};

RTTR_REGISTRATION
{
	FunctorRegistration<SimA>("SimA");
	FunctorRegistration<SimB>("SimB").runAfer("SimA");
}

int main()
{
	SystemManager sm;

	// TODO: this will be done by world loader, using vector of active system type names
	sm.add<InputSystem>();
	auto& simSystem = sm.add<SimulationSystem>();
	sm.add<ModelSystem>();
	auto& renderSystem = sm.add<RenderSystem>();

	// TODO: this will be done by SimulationSystem::configure using data in singleton component describing game simulation settings
	simSystem.add<SimA>();
	simSystem.add<SimB>();

	// TODO: this will be done by somesystem::configure
	renderSystem.find<BatchRenderer>()->addSubsystem<CustomRenderer>();

	sm.configure();

	sm.execute(0.1f); // 1 tick + 0.04
	sm.execute(0.1f); // 2 ticks + 0.02
	sm.execute(0.03f); // 0 ticks + 0.05
	sm.execute(1.f); // many ticks
}
