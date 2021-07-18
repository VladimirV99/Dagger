#pragma once

#include "core/core.h"
#include "core/graphics/window.h"
#include "core/system.h"

#include <imgui/imgui.h>

using namespace dagger;

struct ToolMenuRender
{
};

struct ToolMenuSystem
	: public System
	, public Subscriber<GUIRender>
	, public Publisher<ToolMenuRender>
{
	inline String SystemName() const override
	{
		return "Tool Menu System";
	}

	void RenderGUI();
	void SpinUp() override;
	void WindDown() override;
};