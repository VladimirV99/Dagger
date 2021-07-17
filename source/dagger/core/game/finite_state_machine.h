#pragma once

#include "core/core.h"
#include "core/system.h"

#include <limits>

template<typename States, ENUM_ONLY(States)>
class FSM
{
public:
	struct StateComponent
	{
		Entity entity;
		States currentState;
	};

	class State
	{
		ViewPtr<FSM<States>> m_Parent;
	
	public:
		inline virtual void Enter(StateComponent& /*unused*/) {}
		inline virtual void Run(StateComponent& /*unused*/) {}
		inline virtual void Exit(StateComponent& /*unused*/) {}

		State(FSM<States>* parent_)
			: m_Parent{ parent_ }
		{}
		virtual ~State() = default;

		inline void GoTo(States nextState_, StateComponent& component_)
		{
			assert(m_Parent != nullptr);
			m_Parent->GoTo(nextState_, component_);
		}
	};

	inline void GoTo(States nextState_, StateComponent& component_)
	{
		auto currentState = component_.currentState;
		if (currentState != nextState_)
		{
			m_StatePointers[currentState]->Exit(component_);
			currentState = component_.currentState = nextState_;
			m_StatePointers[currentState]->Enter(component_);
		}
	}

	inline void Run(StateComponent& component_)
	{
		assert(m_StatePointers.contains(component_.currentState));
		m_StatePointers[component_.currentState]->Run(component_);
	}

protected:
	const UInt32 m_StateSize = (UInt32) std::numeric_limits<States>::max();

	Map<States, OwningPtr<State>> m_StatePointers;
};

#define CONNECT_STATE(States_, Class_) m_StatePointers[States_::Class_] = OwningPtr<Class_>(new Class_(this))

#define DEFINE_STATE(Machine_, States_, State_) struct State_ : public State \
{ \
	virtual void Enter(StateComponent& component_) override; \
	virtual void Run(StateComponent& component_) override; \
	virtual void Exit(StateComponent& component_) override; \
	State_(FSM<States_>* parent_) : State(parent_) {} \
};

#define ATTACH_TO_FSM(Machine_, Entity_) Engine::Registry().get_or_emplace<Machine_::StateComponent>(Entity_, Entity_);

#define DEFAULT_ENTER(Machine_, State_) void Machine_::State_::Enter(Machine_::StateComponent&) {}
#define DEFAULT_EXIT(Machine_, State_) void Machine_::State_::Exit(Machine_::StateComponent&) {}
#define DEFAULT_RUN(Machine_, State_) void Machine_::State_::Run(Machine_::StateComponent&) {}
