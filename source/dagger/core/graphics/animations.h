#pragma once

#include "core/core.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/system.h"

#if !defined(NDEBUG)
#include "tools/toolmenu.h"
#endif // !defined(NDEBUG)

#include <glad/glad.h>

using namespace dagger;

class AnimationSystem
	: public System
	, public Subscriber<AssetLoadRequest<Animation>>
#if !defined(NDEBUG)
	, public Publisher<ToolMenuRender>
#endif // !defined(NDEBUG)
{

public:
	inline String SystemName() const override
	{
		return "Animation System";
	}

	static ViewPtr<Animation> Get(String name_);

#if !defined(NDEBUG)
	void RenderToolMenu();
#endif // !defined(NDEBUG)

	void LoadDefaultAssets();
	void OnLoadAsset(AssetLoadRequest<Animation> request_);
	void SpinUp() override;
	void Run() override;
	void WindDown() override;

private:
	Frame LoadFrame(const JSON::json& frameJson_);
};