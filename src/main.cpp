#include "input.hpp"
#include "simulation.hpp"
#include "render.hpp"
#include "model.hpp"
#include "basic_engine.hpp"
#include "feature.hpp"

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

struct MyGameFeature : Feature
{
	void setup(SystemManager& world) override
	{
		auto& simSystem = *world.find<SimulationSystem>();
		simSystem.add<SimA>();
		simSystem.add<SimB>();

		world.find<RenderSystem>()->find<BatchRenderer>()->addSubsystem<CustomRenderer>();
	}
};

RTTR_REGISTRATION
{
	FunctorRegistration<SimA>("SimA");
	FunctorRegistration<SimB>("SimB").runAfer("SimA");
	FunctorRegistration<CustomRenderer>("CustomRenderer");
	FeatureRegistration<MyGameFeature>("MyGameFeature").depends("BasicEngineFeature");
}

int main()
{
	SystemManager sm;

	// TODO: this will be done by world loader, using vector of enabled features
	Feature::enableFeatures(sm, { "BasicEngineFeature", "ModelRenderingFeature", "MyGameFeature" });

	sm.configure();

	sm.execute(0.1f); // 1 tick + 0.04
	sm.execute(0.1f); // 2 ticks + 0.02
	sm.execute(0.03f); // 0 ticks + 0.05
	sm.execute(1.f); // many ticks
}
