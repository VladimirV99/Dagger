#pragma once

#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

namespace editor
{
	enum class ECommonSaveArchetype
	{
		None = 0b00000000,
		Sprite = 0b00000001,
		Transform = 0b00000010,
		Animator = 0b00000100,
		Physics = 0b00001000,
		Camera = 0b00010000,
		// TODO: add new values here
	};

#define IS_ARCHETYPE_SET(in, test) ((in & test) == test)

	inline ECommonSaveArchetype operator|(ECommonSaveArchetype a_, ECommonSaveArchetype b_)
	{
		return static_cast<ECommonSaveArchetype>(static_cast<int>(a_) | static_cast<int>(b_));
	}

	inline ECommonSaveArchetype operator&(ECommonSaveArchetype a_, ECommonSaveArchetype b_)
	{
		return static_cast<ECommonSaveArchetype>(static_cast<int>(a_) & static_cast<int>(b_));
	}
} // namespace editor