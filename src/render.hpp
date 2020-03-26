#pragma once

#include "system_manager.hpp"

class IBatcherSubsystem;

struct Batch
{
	IBatcherSubsystem* subsystem;
	int sortKey;
	int data;
};
using BatchIter = std::vector<Batch>::const_iterator;

struct RenderContext
{
	std::string mSomething;
};

class IRenderSubsystem : public ExecutionGraphElement
{
	//void operator()(float dt, const RenderContext& ctx);
};

class IBatcherSubsystem : public ExecutionGraphElement
{
public:
	//void operator()(BatchRenderer& batcher);
	virtual void evalBatch(BatchIter begin, BatchIter end) = 0;
};

class BatchRenderer : public IRenderSubsystem
{
public:
	template<typename Subsystem>
	void add()
	{
		mGraph.add<Subsystem>();
	}

	void operator()(tf::Subflow& sf, float dt, const RenderContext& ctx);

	void addBatch(IBatcherSubsystem& subsystem, int key, int data);

private:
	ExecutionGraph<IBatcherSubsystem, std::reference_wrapper<BatchRenderer>> mGraph;
	std::vector<Batch> mBatches;
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

	void configure() override;
	void operator()(tf::Subflow& sf, float dt);

	template<typename Subsystem>
	Subsystem* find()
	{
		return mGraph.findElementByType<Subsystem>();
	}

private:
	ExecutionGraph<IRenderSubsystem, float, RenderContext> mGraph;
};
