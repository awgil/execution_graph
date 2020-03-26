#include "render.hpp"

void BatchRenderer::renderInit(const RenderContext& ctx)
{
	printf("Batcher init: %s\n", ctx.mSomething.c_str());
}

void BatchRenderer::render(tf::Subflow& sf, float, const RenderContext&)
{
	// first let all registered subsystems fill their batches
	auto [A, B] = mGraph.execute(sf, *this);

	// then sort batches (TODO: parallel sort)
	auto C = sf.emplace([this]() {
		std::sort(mBatches.begin(), mBatches.end(), [](const Batch& l, const Batch& r) { return l.sortKey < r.sortKey; });
	});
	C.succeed(B);

	// finally, render them in order
	auto D = sf.emplace([this]() {
		BatchIter rangeBegin = mBatches.begin();
		while (rangeBegin != mBatches.end())
		{
			BatchIter rangeEnd = rangeBegin + 1;
			while (rangeEnd != mBatches.end() && rangeEnd->subsystem == rangeBegin->subsystem)
				++rangeEnd;
			rangeBegin->subsystem->evalBatch(rangeBegin, rangeEnd);
			rangeBegin = rangeEnd;
		}
		mBatches.clear();
	});
	D.succeed(C);
}

void RenderSystem::configure()
{
	add<BatchRenderer>();
}

void RenderSystem::execute(tf::Subflow& sf, float dt)
{
	auto [A, B] = mInitGraph.execute(sf, RenderContext{ "init context" });
	auto [C, D] = mRenderGraph.execute(sf, dt, RenderContext{ "render context" });
	C.succeed(B);
}

// TODO: RTTR registration, edges defined here...

void BatchRenderer::addBatch(IBatcherSubsystem& subsystem, int key, int data)
{
	mBatches.push_back({ &subsystem, key, data });
}
