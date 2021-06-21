#pragma once

#include "core/graphics/window.h"
#include "core/system.h"
#include "core/core.h"

#include <chrono>

using namespace dagger;
namespace pandemic_shop{

    class CollisionDetectionSystem 
	: public System
	, public Subscriber<GUIRender, NextFrame>
{
	UInt64 m_LastFrameCounter;
	UInt64 m_FrameCounter;
	Float32 m_DeltaSum;

	void Tick();
	void RenderGUI();
		
public:
	inline String SystemName() { return "Collision Detection System"; }

	void SpinUp() override;
	void Run() override;
	void WindDown() override;
};


}