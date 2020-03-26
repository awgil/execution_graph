#include "model.hpp"

void ModelSystem::configure()
{
	auto* renderSystem = mMgr.find<RenderSystem>();
	assert(renderSystem); // it's an error to add ModelSystem without RenderSystem?..
	auto* batcher = renderSystem->find<BatchRenderer>();
	assert(batcher);

	// this is the same style of registration as used currently by corex
	batcher->addSubsystem<ModelRenderer>();
}

void ModelSystem::execute(tf::Subflow&, float dt)
{
	printf("Updating model transforms, %f passed...\n", dt);
}

void ModelRenderer::prepBatch(BatchRenderer& batcher)
{
	printf("Preparing model batches\n");
	// below is just an example...
	batcher.addBatch(*this, 4, 40);
	batcher.addBatch(*this, 1, 10);
	batcher.addBatch(*this, 1, 11);
	batcher.addBatch(*this, 2, 20);
}

void ModelRenderer::evalBatch(BatchIter begin, BatchIter end)
{
	printf("Rendering models:\n");
	for (auto i = begin; i != end; ++i)
		printf("- model %d\n", i->data);
}

RTTR_REGISTRATION
{
	FunctorRegistration<ModelSystem>("ModelSystem")
		.runBefore("RenderSystem");

	FunctorRegistration<ModelRenderer>("ModelRenderer");
}
