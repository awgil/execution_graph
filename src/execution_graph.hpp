#pragma once

#include <taskflow/taskflow.hpp>
#include <tuple>

template<typename T>
using uninit_storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

template<typename... Args>
class ExecutionGraph
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
			static_assert("Unexpected callable signature");
	}

	template<typename... IArgs>
	void execute(tf::Executor& executor, IArgs &&... arguments)
	{
		new(&mArgsStorage) arg_tuple(std::forward<IArgs>(arguments)...);
		executor.run(mTF).get();
		args().~arg_tuple();
	}

	template<typename... IArgs>
	auto execute_in(tf::Subflow& subflow, tf::Task predecessor, tf::Task successor, IArgs &&... arguments)
	{
		new(&mArgsStorage) arg_tuple(std::forward<IArgs>(arguments)...);
		auto A = subflow.composed_of(mTF);
		auto B = subflow.emplace([this]() { args().~arg_tuple(); });
		A.precede(B);
		if (!predecessor.empty())
			A.succeed(predecessor);
		if (!successor.empty())
			B.precede(successor);
		return std::make_pair(A, B);
	}

private:
	tf::Taskflow mTF;
	uninit_storage_t<arg_tuple> mArgsStorage;
};
