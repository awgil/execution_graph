#pragma once

#include "render.hpp"

class ModelSystem : public ISystem
{
public:
	using ISystem::ISystem;
	void configure() override;
	void operator()(float dt);
};

class ModelRenderer : public IBatcherSubsystem
{
public:
	void operator()(BatchRenderer& batcher);
	void evalBatch(BatchIter begin, BatchIter end) override;
};
