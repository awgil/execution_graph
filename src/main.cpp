#include "execution_graph.hpp"

class ISystem
{
public:
	virtual ~ISystem() = default;
};

class SystemManager
{
	using Systems = ExecutionGraph<float>;
public:
	template<typename System>
	std::tuple<System&, tf::Task> add()
	{
		static_assert(std::is_base_of_v<ISystem, System>);
		auto& system = static_cast<System&>(*mSystems.emplace_back(new System));
		auto task = mGraph.add(std::ref(system));
		return { system, task };
	}

	void execute(float dt)
	{
		mGraph.execute(mExecutor, dt);
	}

private:
	Systems mGraph;
	tf::Executor mExecutor;
	std::vector<std::unique_ptr<ISystem>> mSystems;
};

class InputSystem : public ISystem
{
public:
	void operator()(float dt)
	{
		printf("Input; %f passed since prev frame\n", dt);
	}
};

class SimulationSystem : public ISystem
{
	using Subsystems = ExecutionGraph<>;
public:
	template<typename Callable>
	tf::Task addSubsystem(Callable&& c)
	{
		return mGraph.add(std::forward<Callable>(c));
	}

	void operator()(tf::Subflow& sf, float dt)
	{
		mRemaining += dt;
		tf::Task prev;
		while (mRemaining >= kFrameLength)
		{
			mRemaining -= kFrameLength;
			prev = mGraph.execute_in(sf, prev, {}).second;
		}
	}

private:
	static constexpr float kFrameLength = 0.06f;
	float mRemaining = 0.f;
	Subsystems mGraph;
};

class RenderSystem : public ISystem
{
	using Subsystems = ExecutionGraph<float, std::string>;
public:
	// we assume all render subsystems can run in parallel => don't return task, since no dependencies are needed
	template<typename Callable>
	void addSubsystem(Callable&& c)
	{
		mGraph.add(c);
	}

	void operator()(tf::Subflow& sf, float dt)
	{
		mGraph.execute_in(sf, {}, {}, dt, "render context");
	}

private:
	Subsystems mGraph;
};

int main()
{
	SystemManager sm;

	auto [inputSystem, inputTask] = sm.add<InputSystem>();
	auto [simSystem, simTask] = sm.add<SimulationSystem>();
	auto [renderSystem, renderTask] = sm.add<RenderSystem>();
	inputTask.precede(simTask);
	simTask.precede(renderTask);

	auto simA = simSystem.addSubsystem([]() { printf("Simulation A\n"); });
	auto simB = simSystem.addSubsystem([]() { printf("Simulation B\n"); });
	simA.precede(simB);

	renderSystem.addSubsystem([](float, const std::string& msg) { printf("Render A: %s\n", msg.c_str()); });
	renderSystem.addSubsystem([](float, const std::string& msg) { printf("Render B: %s\n", msg.c_str()); });
	renderSystem.addSubsystem([](float, const std::string& msg) { printf("Render C: %s\n", msg.c_str()); });

	sm.execute(0.1f); // 1 tick + 0.04
	sm.execute(0.1f); // 2 ticks + 0.02
	sm.execute(0.03f); // 0 ticks + 0.05
	sm.execute(1.f); // many ticks
}
