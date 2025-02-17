#pragma once

#include "core/core.h"
#include "core/graphics/window.h"
#include "core/system.h"

#include <chrono>

using namespace dagger;

class DiagnosticSystem
	: public System
	, public Subscriber<GUIRender, NextFrame>
{
	UInt64 m_LastFrameCounter;
	UInt64 m_FrameCounter;
	Float32 m_DeltaSum;

#if defined(MEASURE_SYSTEMS)
	Float32 m_SystemTimeCounter;
	Map<String, Float32> m_SystemStats;
	void ReceiveSystemStats(SystemRunStats stats_);
#endif // defined(MEASURE_SYSTEMS)

	void Tick();
	void RenderGUI() const;

public:
	inline String SystemName() const override
	{
		return "Diagnostic System";
	}

	void SpinUp() override;
	void Run() override;
	void WindDown() override;
};
