#pragma once
#include "core/core.h"
#include "core/engine.h"
#include "core/system.h"

using namespace dagger;

struct Crosshair
{
	Entity crosshairSprite;		   // used for getting a Sprite component for our crosshair to be drawn on screen
	Float32 angle {0.f};		   // current angle of the crosshair (between the X axis and the crosshair position)
	Float32 playerDistance {20.f}; // distance of the crosshair relative to the player position
};

class AimingSystem : public System
{
public:
	inline String SystemName() const override
	{
		return "Aiming System";
	}

	void Run() override;
};
