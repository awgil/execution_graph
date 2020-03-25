#pragma once

#include "render.hpp"

class ModelSystem : public ISystem
{
public:
	using ISystem::ISystem;
	void configure() override;
	void operator()(float dt);
};

class ModelRenderer : public IRenderSubsystem
{
public:
	void operator()(float dt, const RenderContext& ctx);
};
