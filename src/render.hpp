#pragma once

#include "system_manager.hpp"

struct RenderContext
{
	std::string mSomething;
};

class IRenderSubsystem : public ExecutionGraphElement
{
	//void operator()(float dt, const RenderContext& ctx);
};

class RenderSystem : public ISystem
{
public:
	using ISystem::ISystem;

	// we assume all render subsystems can run in parallel => don't return task, since no dependencies are needed
	template<typename Subsystem>
	void add()
	{
		mGraph.add<Subsystem>();
	}

	void operator()(tf::Subflow& sf, float dt);

private:
	ExecutionGraph<IRenderSubsystem, float, RenderContext> mGraph;
};
