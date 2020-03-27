#include "simulation.hpp"

void SimulationSystem::configure()
{
	printf("Sim configure\n");
	mSubsystems.buildTaskflow(mGraph, &ISimulationSubsystem::simulateFrame);
}

void SimulationSystem::execute(tf::Subflow& sf, float dt)
{
	mRemaining += dt;
	tf::Task prev;
	while (mRemaining >= kFrameLength)
	{
		mRemaining -= kFrameLength;
		auto [A, B] = mGraph.execute(sf);
		if (!prev.empty())
			prev.precede(A);
		prev = B;
	}
}

RTTR_REGISTRATION
{
	FunctorRegistration<SimulationSystem>("SimulationSystem")
		.runAfer("InputSystem")
		.runBefore("ModelSystem", "RenderSystem");
}
