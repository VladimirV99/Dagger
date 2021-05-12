#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"
#include "core/graphics/animation.h"

using namespace dagger;

namespace atonement {

	enum struct ECharStates
	{
		Idle,
		Walking,
		JumpWindup,
		JumpWinddown,
		Dashing
	};

	struct CharControllerFSM : public FSM<ECharStates>
	{
		DEFINE_STATE(CharControllerFSM, ECharStates, Idle);
		DEFINE_STATE(CharControllerFSM, ECharStates, Walking);
		DEFINE_STATE(CharControllerFSM, ECharStates, JumpWindup);
		DEFINE_STATE(CharControllerFSM, ECharStates, JumpWinddown);
		DEFINE_STATE(CharControllerFSM, ECharStates, Dashing);

		CharControllerFSM()
		{
			CONNECT_STATE(ECharStates, Idle);
			CONNECT_STATE(ECharStates, Walking);
			CONNECT_STATE(ECharStates, JumpWindup);
			CONNECT_STATE(ECharStates, JumpWinddown);
			CONNECT_STATE(ECharStates, Dashing);
		}

		void OnAnimationEnd(ViewPtr<Animation> animation);

		static Bool canDash(Entity entityID);

		Float32 fallenInLastFrame{ 0 };
	};

}