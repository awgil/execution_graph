#pragma once

#include "feature.hpp"

class SystemManager;

// basic engine features
struct BasicEngineFeature : Feature
{
	void setup(SystemManager& world) override;
};
