#include "model.hpp"

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

void ModelRenderingFeature::setup(SystemManager& world)
{
	world.add<ModelSystem>();
	world.find<RenderSystem>()->find<BatchRenderer>()->addSubsystem<ModelRenderer>();
}

RTTR_REGISTRATION
{
	FunctorRegistration<ModelSystem>("ModelSystem")
		.runBefore("RenderSystem");

	FunctorRegistration<ModelRenderer>("ModelRenderer");

	FeatureRegistration<ModelRenderingFeature>("ModelRenderingFeature").depends("BasicEngineFeature");
}
