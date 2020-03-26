#pragma once

#include "system_manager.hpp"

class IBatcherSubsystem;
class BatchRenderer;

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
public:
	virtual void render(tf::Subflow& sf, float dt, const RenderContext& ctx) = 0;

	void operator()(tf::Subflow& sf, float dt, const RenderContext& ctx) { render(sf, dt, ctx); }
};

class IBatcherSubsystem : public ExecutionGraphElement
{
public:
	virtual void prepBatch(BatchRenderer& batcher) = 0;
	virtual void evalBatch(BatchIter begin, BatchIter end) = 0;

	void operator()(BatchRenderer& batcher) { prepBatch(batcher); }
};

class BatchRenderer : public IRenderSubsystem
{
public:
	template<typename Subsystem>
	void addSubsystem()
	{
		mGraph.add<Subsystem>();
	}

	void addBatch(IBatcherSubsystem& subsystem, int key, int data);

	void render(tf::Subflow& sf, float dt, const RenderContext& ctx) override;

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
	void execute(tf::Subflow& sf, float dt) override;

	template<typename Subsystem>
	Subsystem* find()
	{
		return mGraph.findElementByType<Subsystem>();
	}

private:
	ExecutionGraph<IRenderSubsystem, float, RenderContext> mGraph;
};
