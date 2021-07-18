#pragma once

#include "core/core.h"
#include "core/system.h"
#include "window.h"

using namespace dagger;

class GUISystem
	: public System
	, public Subscriber<PreRender, ToolRender>
	, public Publisher<GUIRender>
{
	void OnPreRender();
	void OnToolRender();

public:
	inline String SystemName() const override
	{
		return "GUI System";
	}

	void SpinUp() override;
	void WindDown() override;
};