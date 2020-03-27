#pragma once

#include "render.hpp"
#include "feature.hpp"

class ModelSystem : public System<ModelSystem>
{
public:
	using System::System;
	void execute(tf::Subflow& sf, float dt) override;
};

class ModelRenderer : public IBatcherSubsystem
{
	RTTR_ENABLE(IBatcherSubsystem);
public:
	void prepBatch(BatchRenderer& batcher) override;
	void evalBatch(BatchIter begin, BatchIter end) override;
};

struct ModelRenderingFeature : Feature
{
	void setup(SystemManager& world) override;
};
