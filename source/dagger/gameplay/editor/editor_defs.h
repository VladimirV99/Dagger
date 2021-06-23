#pragma once

#include "core/core.h"

enum class ECommonSaveArchetype
{
    None         = 0b00000000,
    Sprite       = 0b00000001,
    Transform    = 0b00000010,
    Animator     = 0b00000100,
    Physics      = 0b00001000,
    Bouncy       = 0b00010000,
    Deadly       = 0b00100000,
    Interactable = 0b01000000,
    // todo: add new values here
};

#define IS_ARCHETYPE_SET(in, test) ((in & test) == test)

inline ECommonSaveArchetype operator|(ECommonSaveArchetype a, ECommonSaveArchetype b)
{
    return static_cast<ECommonSaveArchetype>(static_cast<int>(a) | static_cast<int>(b));
}

inline ECommonSaveArchetype operator&(ECommonSaveArchetype a, ECommonSaveArchetype b)
{
    return static_cast<ECommonSaveArchetype>(static_cast<int>(a) & static_cast<int>(b));
}

struct EditorFocus
{
    Bool dirty;
};

struct EditorFocusTarget
{
    Entity entity;
    String name;
};
