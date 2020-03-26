#pragma once

#include "execution_graph.hpp"
#include <rttr/rttr_enable.h>

class SystemManager;

class ISystem
{
	RTTR_ENABLE();
public:
	ISystem(SystemManager& mgr) : mMgr(mgr) {}
	virtual ~ISystem() = default;

	virtual void configure() {}
	virtual void execute(tf::Subflow& sf, float dt) = 0;

protected:
	SystemManager& mMgr;
};

class SystemManager
{
public:
	// TODO: call by world loader based on list of systems in world
	// take type name string instead of <Type> and access RTTR db
	// return nothing and add no dependency edges - this will be done by configure...
	template<typename System>
	System& add()
	{
		return mSystems.add<System>(*this);
	}

	void configure()
	{
		mSystems.buildTaskflow(mGraph, &ISystem::execute);

		for (auto& system : mSystems.functors())
		{
			system->configure();
		}
	}

	void execute(float dt)
	{
		mGraph.execute(mExecutor, dt);
	}

	template<typename System>
	System* find()
	{
		return mSystems.findByType<System>();
	}

private:
	FunctorList<ISystem> mSystems;
	tf::Executor mExecutor;
	TaskflowWithArgs<float> mGraph;
};
