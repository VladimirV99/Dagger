#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"

enum struct EPandemicCharacterStates { Idle, Running };
enum struct EDirection { Up = 1, Right = 2, Left = 3, Down = 4 };

static inline String GetNameFromDirection(const EDirection dir_)
{
    switch (dir_)
    {
    case EDirection::Up: return "_FRONT"; 
    case EDirection::Right: return "_RIGHT";
    case EDirection::Left: return "_LEFT"; 
    case EDirection::Down: default: return "_DOWN";
    }
}

static inline EDirection GetDirectionFromVector(const Vector2 hv_)
{
    if (hv_.x == 0 && hv_.y == -1) return EDirection::Up;
    else if (hv_.x == 1 && hv_.y == 0) return EDirection::Right;
    else if (hv_.x == -1 && hv_.y == 0) return EDirection::Left;
    else if (hv_.x == 0 && hv_.y == 1) return EDirection::Down;
    else if (hv_.y == -1) return EDirection::Up;
    else if (hv_.y == 1) return EDirection::Down;
    else if (hv_.x == -1) return EDirection::Left;
    else if (hv_.x == 1) return EDirection::Right;
}

struct PandemicCharacterControllerFSM : public FSM<EPandemicCharacterStates> {
    DEFINE_STATE(PandemicCharacterControllerFSM, EPandemicCharacterStates, Idle);
    DEFINE_STATE(PandemicCharacterControllerFSM, EPandemicCharacterStates, Running);

    PandemicCharacterControllerFSM() {
        CONNECT_STATE(EPandemicCharacterStates, Idle);
        CONNECT_STATE(EPandemicCharacterStates, Running);
    }
};
