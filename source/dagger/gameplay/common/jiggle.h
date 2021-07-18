#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

class JiggleSystem : public System
{
	inline String SystemName() const override
	{
		return "Jiggle System";
	}

	void Run() override;
};