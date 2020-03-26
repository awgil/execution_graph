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

class IRenderSubsystem
{
	RTTR_ENABLE();
public:
	virtual void renderInit(const RenderContext& ctx) = 0;
	virtual void render(tf::Subflow& sf, float dt, const RenderContext& ctx) = 0;
};

class IBatcherSubsystem
{
	RTTR_ENABLE();
public:
	virtual void prepBatch(BatchRenderer& batcher) = 0;
	virtual void evalBatch(BatchIter begin, BatchIter end) = 0;
};

class BatchRenderer : public IRenderSubsystem
{
	RTTR_ENABLE(IRenderSubsystem);
public:
	template<typename Subsystem>
	void addSubsystem()
	{
		mSubsystems.add<Subsystem>();
	}

	void configure();

	void addBatch(IBatcherSubsystem& subsystem, int key, int data);

	void renderInit(const RenderContext& ctx) override;
	void render(tf::Subflow& sf, float dt, const RenderContext& ctx) override;

private:
	FunctorList<IBatcherSubsystem> mSubsystems;
	TaskflowWithArgs<std::reference_wrapper<BatchRenderer>> mGraph;
	std::mutex mBatchesMutex;
	std::vector<Batch> mBatches;
};

class RenderSystem : public ISystem
{
	RTTR_ENABLE(ISystem);
public:
	RenderSystem(SystemManager& mgr);

	template<typename Subsystem>
	void add()
	{
		mSubsystems.add<Subsystem>();
	}

	void configure() override;
	void execute(tf::Subflow& sf, float dt) override;

	template<typename Subsystem>
	Subsystem* find()
	{
		return mSubsystems.findByType<Subsystem>();
	}

private:
	FunctorList<IRenderSubsystem> mSubsystems;
	TaskflowWithArgs<RenderContext> mInitGraph;
	TaskflowWithArgs<float, RenderContext> mRenderGraph;
};
