#include "feature.hpp"

void Feature::enableFeatures(SystemManager& world, const std::vector<std::string>& features)
{
	std::unordered_set<rttr::type> enabledFeatures;
	for (auto& f : features)
	{
		auto t = rttr::type::get_by_name(f);
		if (enabledFeatures.find(t) != enabledFeatures.end())
			continue; // feature is already enabled

		// make sure all features we depend on are enabled
		if (auto meta = t.get_metadata(rttr::string_view("DEPENDS")))
		{
			for (auto& dep : meta.get_value<std::vector<std::string>>())
			{
				rttr::type u = rttr::type::get_by_name(dep);
				if (enabledFeatures.find(u) != enabledFeatures.end())
					continue; // dependency already satisfied

				// TODO: maybe automatically enable (recursively) instead?
				assert(!"Trying to enable feature without dependency");
			}
		}

		enabledFeatures.insert(t);
		t.create().get_value<Feature>().setup(world);
	}
}
