#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"

enum struct EKarenStates { Idle, Running };

struct KarenControllerFSM : public FSM<EKarenStates> {
    DEFINE_STATE(KarenControllerFSM, EKarenStates, Idle);
    DEFINE_STATE(KarenControllerFSM, EKarenStates, Running);

    KarenControllerFSM() {
        CONNECT_STATE(EKarenStates, Idle);
        CONNECT_STATE(EKarenStates, Running);
    }
};
