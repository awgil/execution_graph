#pragma once

#include <assert.h>
#include <rttr/registration>

class SystemManager;

class Feature
{
public:
	virtual void setup(SystemManager& world) = 0;

	static void enableFeatures(SystemManager& world, const std::vector<std::string>& features);
};

template<typename T>
class FeatureRegistration
{
public:
	FeatureRegistration(rttr::string_view name)
		: mClass(name)
	{
		mClass.constructor()(rttr::policy::ctor::as_object);
	}

	template<typename... Strings>
	FeatureRegistration& depends(Strings&&... classes)
	{
		mClass(rttr::metadata(rttr::string_view("DEPENDS"), std::vector<std::string>{ std::forward<Strings>(classes)... }));
		return *this;
	}

private:
	rttr::registration::class_<T> mClass;
};
