#pragma once

#include "render.hpp"

class ModelSystem : public ISystem
{
public:
	using ISystem::ISystem;
	void configure() override;
	void execute(tf::Subflow& sf, float dt) override;
};

class ModelRenderer : public IBatcherSubsystem
{
public:
	void prepBatch(BatchRenderer& batcher) override;
	void evalBatch(BatchIter begin, BatchIter end) override;
};
