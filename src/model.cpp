#include "model.hpp"

void ModelSystem::configure()
{
	auto* renderSystem = mMgr.find<RenderSystem>();
	assert(renderSystem); // it's an error to add ModelSystem without RenderSystem?..

	// this is the same style of registration as used currently by corex
	renderSystem->add<ModelRenderer>();
}

void ModelSystem::operator()(float dt)
{
	printf("Updating model transforms, %f passed...\n", dt);
}

void ModelRenderer::operator()(float dt, const RenderContext& ctx)
{
	printf("Rendering models: %f, %s\n", dt, ctx.mSomething.c_str());
}
