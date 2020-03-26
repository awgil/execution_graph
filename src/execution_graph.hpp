#pragma once

#include <taskflow/taskflow.hpp>
#include <rttr/type>
#include <rttr/registration>
#include <tuple>

template<typename T>
using uninit_storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

template<typename... Args>
class TaskflowWithArgs
{
public:
	using arg_tuple = std::tuple<Args...>;

	arg_tuple& args() { return reinterpret_cast<arg_tuple&>(mArgsStorage); }

public:
	template<typename Callable>
	tf::Task add(Callable&& c)
	{
		if constexpr (std::is_invocable_v<Callable, Args...>)
			return mTF.emplace([this, c = std::forward<Callable>(c)]() mutable { std::apply(c, args()); });
		else if constexpr (std::is_invocable_v<Callable, tf::Subflow&, Args...>)
			return mTF.emplace([this, c = std::forward<Callable>(c)](tf::Subflow& sf) mutable { std::apply(c, std::tuple_cat(std::tie(sf), args())); });
		else
			static_assert(false, "Unexpected callable signature");
	}

	void clear()
	{
		mTF.clear();
	}

	template<typename... IArgs>
	void execute(tf::Executor& executor, IArgs &&... arguments)
	{
		new(&mArgsStorage) arg_tuple(std::forward<IArgs>(arguments)...);
		executor.run(mTF).get();
		args().~arg_tuple();
	}

	template<typename... IArgs>
	auto execute(tf::Subflow& subflow, IArgs &&... arguments)
	{
		new(&mArgsStorage) arg_tuple(std::forward<IArgs>(arguments)...);
		auto A = subflow.composed_of(mTF);
		auto B = subflow.emplace([this]() { args().~arg_tuple(); });
		A.precede(B);
		return std::make_pair(A, B);
	}

private:
	tf::Taskflow mTF;
	uninit_storage_t<arg_tuple> mArgsStorage;
};

template<typename Base>
class FunctorList
{
public:
	Base& add(std::unique_ptr<Base>&& func)
	{
		mTypeToIndex[rttr::type::get(*func)] = mFunctors.size();
		return *mFunctors.emplace_back(std::move(func));
	}

	template<typename Derived, typename... CtorArgs>
	Derived& add(CtorArgs&&... ctorArgs)
	{
		static_assert(std::is_base_of_v<Base, Derived>);
		return static_cast<Derived&>(add(std::make_unique<Derived>(std::forward<CtorArgs>(ctorArgs)...)));
	}

	auto& functors() { return mFunctors; }

	template<typename T>
	T* findByType()
	{
		auto i = mTypeToIndex.find(rttr::type::get<T>());
		return i != mTypeToIndex.end() ? static_cast<T*>(mFunctors[i->second].get()) : nullptr;
	}

	template<typename... Args, typename... IArgs>
	std::vector<tf::Task> buildTaskflow(TaskflowWithArgs<Args...> &tf, void (Base::*func)(IArgs...))
	{
		std::vector<tf::Task> tasks;
		tasks.reserve(mFunctors.size());

		tf.clear();
		for (auto& f : mFunctors)
		{
			tasks.push_back(tf.add([&fn = *f, func](IArgs... args) { (fn.*func)(std::forward<IArgs>(args)...); }));
		}

		auto getExistingEdges = [this](rttr::type t, rttr::string_view metaName) {
			std::vector<size_t> res;
			if (auto meta = t.get_metadata(metaName))
			{
				for (auto& edge : meta.get_value<std::vector<std::string>>())
				{
					rttr::type u = rttr::type::get_by_name(edge);
					auto iu = mTypeToIndex.find(u);
					if (iu == mTypeToIndex.end())
						continue; // skip edge - note that it's not ideal - consider A->B->C with B not added
					res.push_back(iu->second);
				}
			}
			return res;
		};

		for (size_t i = 0; i < mFunctors.size(); ++i)
		{
			rttr::type t = rttr::type::get(*mFunctors[i]);
			for (auto j : getExistingEdges(t, "EDGE_RUN_BEFORE"))
				tasks[i].precede(tasks[j]);
			for (auto j : getExistingEdges(t, "EDGE_RUN_AFTER"))
				tasks[i].succeed(tasks[j]);
		}

		return tasks;
	}

private:
	std::vector<std::unique_ptr<Base>> mFunctors;
	std::unordered_map<rttr::type, size_t> mTypeToIndex;
};

template<typename T>
class FunctorRegistration
{
public:
	FunctorRegistration(rttr::string_view name)
		: mClass(name)
	{
	}

	template<typename... Strings>
	FunctorRegistration& runBefore(Strings &&... classes)
	{
		mClass(rttr::metadata(rttr::string_view("EDGE_RUN_BEFORE"), std::vector<std::string>{ std::forward<Strings>(classes)... }));
		return *this;
	}

	template<typename... Strings>
	FunctorRegistration& runAfer(Strings&&... classes)
	{
		mClass(rttr::metadata(rttr::string_view("EDGE_RUN_AFTER"), std::vector<std::string>{ std::forward<Strings>(classes)... }));
		return *this;
	}

private:
	rttr::registration::class_<T> mClass;
};
