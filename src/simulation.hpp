#pragma once

#include "system_manager.hpp"

class ISimulationSubsystem : public ExecutionGraphElement
{
public:
	virtual void simulateFrame() = 0;

	void operator()() { simulateFrame(); }
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
	void execute(tf::Subflow& sf, float dt) override;

private:
	static constexpr float kFrameLength = 0.06f;
	float mRemaining = 0.f;
	ExecutionGraph<ISimulationSubsystem> mGraph;
};
