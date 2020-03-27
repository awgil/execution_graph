#pragma once

#include "system_manager.hpp"

class ISimulationSubsystem
{
	RTTR_ENABLE();
public:
	virtual ~ISimulationSubsystem() = default;
	virtual void simulateFrame() = 0;
};

class SimulationSystem : public System<SimulationSystem>
{
public:
	using System::System;

	template<typename Subsystem>
	Subsystem& add()
	{
		return mSubsystems.add<Subsystem>();
	}

	void configure() override;
	void execute(tf::Subflow& sf, float dt) override;

private:
	static constexpr float kFrameLength = 0.06f;
	float mRemaining = 0.f;
	FunctorList<ISimulationSubsystem> mSubsystems;
	TaskflowWithArgs<> mGraph;
};
