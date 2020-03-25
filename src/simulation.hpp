#pragma once

#include "system_manager.hpp"

class ISimulationSubsystem : public ExecutionGraphElement
{
	//void operator()();
};

class SimulationSystem : public ISystem
{
public:
	using ISystem::ISystem;

	// TODO: should be called inside configure
	template<typename Subsystem>
	Subsystem& add()
	{
		return mGraph.add<Subsystem>();
	}

	void configure() override;
	void operator()(tf::Subflow& sf, float dt);

private:
	static constexpr float kFrameLength = 0.06f;
	float mRemaining = 0.f;
	ExecutionGraph<ISimulationSubsystem> mGraph;
};
