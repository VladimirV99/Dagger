#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"

enum struct EPandemicCharacterStates
{
	Idle,
	Running
};

struct PlatformerCharacterControllerFSM : public FSM<EPandemicCharacterStates>
{
	DEFINE_STATE(PlatformerCharacterControllerFSM, EPandemicCharacterStates, Idle);
	DEFINE_STATE(PlatformerCharacterControllerFSM, EPandemicCharacterStates, Running);

	PlatformerCharacterControllerFSM()
	{
		CONNECT_STATE(EPandemicCharacterStates, Idle);
		CONNECT_STATE(EPandemicCharacterStates, Running);
	}
};