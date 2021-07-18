#pragma once

#if defined(DAGGER_DEBUG)

#include "core/core.h"
#include "core/system.h"
#include "tools/toolmenu.h"

using namespace dagger;

namespace ping_pong
{
	class PingPongTools
		: public System
		, public Publisher<ToolMenuRender>
	{
		bool m_RestartGame = false;
		bool m_GenerateBall = false;

	public:
		inline String SystemName() const override
		{
			return "Ping Pong Tools System";
		}

		void SpinUp() override;
		void Run() override;
		void WindDown() override;

	protected:
		void RenderToolMenu();
		void OnEndOfFrame();
	};
} // namespace ping_pong

#endif // defined(DAGGER_DEBUG)
