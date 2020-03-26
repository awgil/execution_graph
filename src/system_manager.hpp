#pragma once

#include "execution_graph.hpp"

class SystemManager;

class ISystem : public ExecutionGraphElement
{
public:
	ISystem(SystemManager& mgr) : mMgr(mgr) {}

	virtual void configure() {}

	//void operator()(float dt);

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
		return mGraph.add<System>(*this);
	}

	void configure()
	{
		// TODO: use metadata from RTTR db to configure edges between systems
		for (auto& system : mGraph.elements())
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
		return mGraph.findElementByType<System>();
	}

private:
	tf::Executor mExecutor;
	ExecutionGraph<ISystem, float> mGraph;
};
