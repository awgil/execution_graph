#include "input.hpp"
#include "simulation.hpp"
#include "render.hpp"
#include "model.hpp"

struct SimA : public ISimulationSubsystem
{
	void operator()()
	{
		printf("Simulation A\n");
	}
};

struct SimB : public ISimulationSubsystem
{
	void operator()()
	{
		printf("Simulation B\n");
	}
};

struct CustomRenderer : public IBatcherSubsystem
{
	void operator()(BatchRenderer& batcher)
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

int main()
{
	SystemManager sm;

	// TODO: this will be done by world loader, using vector of active system type names
	auto& inputSystem = sm.add<InputSystem>();
	auto& simSystem = sm.add<SimulationSystem>();
	auto& renderSystem = sm.add<RenderSystem>();
	auto& modelSystem = sm.add<ModelSystem>();

	// TODO: this will be done by SystemManager::configure using statically-defined edges between systems
	inputSystem.task().precede(simSystem.task());
	simSystem.task().precede(renderSystem.task());
	simSystem.task().precede(modelSystem.task());
	modelSystem.task().precede(renderSystem.task());

	// TODO: this will be done by SimulationSystem::configure using data in singleton component describing game simulation settings
	auto& simA = simSystem.add<SimA>();
	auto& simB = simSystem.add<SimB>();
	simA.task().precede(simB.task());

	sm.configure();

	// TODO: this will be done by somesystem::configure
	renderSystem.find<BatchRenderer>()->add<CustomRenderer>();

	sm.execute(0.1f); // 1 tick + 0.04
	sm.execute(0.1f); // 2 ticks + 0.02
	sm.execute(0.03f); // 0 ticks + 0.05
	sm.execute(1.f); // many ticks
}
