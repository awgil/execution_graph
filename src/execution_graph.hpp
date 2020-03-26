#pragma once

#include <taskflow/taskflow.hpp>
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

class ExecutionGraphElement
{
public:
	virtual ~ExecutionGraphElement() = default;

	tf::Task task() const { return mTask; }

private:
	tf::Task mTask;

	template<typename, typename...> friend class ExecutionGraph;
};

template<typename Base, typename... Args>
class ExecutionGraph : private TaskflowWithArgs<Args...>
{
	static_assert(std::is_base_of_v<ExecutionGraphElement, Base>);

public:
	template<typename Element, typename... CtorArgs>
	Element& add(CtorArgs &&... ctorArgs)
	{
		static_assert(std::is_base_of_v<Base, Element>);
		auto& element = static_cast<Element&>(*mElements.emplace_back(new Element(std::forward<CtorArgs>(ctorArgs)...)));
		element.mTask = TaskflowWithArgs<Args...>::add(std::ref(element));
		return element;
	}

	Base& add(std::unique_ptr<Base>&& elem)
	{
		auto& element = *mElements.emplace_back(std::move(elem));
		element.mTask = TaskflowWithArgs<Args...>::add(std::ref(element));
		return element;
	}

	using TaskflowWithArgs<Args...>::execute;

	auto& elements() { return mElements; }

	template<typename Element>
	Element* findElementByType()
	{
		// this should of course use RTTR and fast lookups...
		for (auto& e : mElements)
		{
			if (auto* cast = dynamic_cast<Element*>(e.get()))
			{
				return cast;
			}
		}
		return nullptr;
	}

private:
	std::vector<std::unique_ptr<Base>> mElements;
};
