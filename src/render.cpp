#include "render.hpp"

void RenderSystem::operator()(tf::Subflow& sf, float dt)
{
	mGraph.execute(sf, dt, RenderContext{ "render context" });
}

// TODO: RTTR registration, edges defined here...
