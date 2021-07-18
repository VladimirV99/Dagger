#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

struct Transform
{
	Vector3 position {0, 0, 0};
};

class TransformSystem : public System
{
	inline String SystemName() const override
	{
		return "Transform System";
	}

	void Run() override;
};