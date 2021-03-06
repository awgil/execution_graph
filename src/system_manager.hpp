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

template<typename T>
struct System : ISystem
{
	using ISystem::ISystem;

	auto& self() { return static_cast<T&>(*this); }
	auto& self() const { return static_cast<const T&>(*this); }

	RTTR_CRTP_ENABLE(ISystem);
};

class SystemManager
{
public:
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
