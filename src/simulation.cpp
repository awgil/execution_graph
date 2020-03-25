#include "simulation.hpp"

void SimulationSystem::configure()
{
	printf("Sim configure\n");
	// TODO: look up singleton component describing simulation settings, use vector of sim subsystem types to add nodes
	// then use RTTR db to add edges between active subsystems
}

void SimulationSystem::operator()(tf::Subflow& sf, float dt)
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

// TODO: RTTR registration, edges defined here...
