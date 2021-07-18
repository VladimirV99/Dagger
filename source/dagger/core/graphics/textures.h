#pragma once

#include "core/core.h"
#include "core/system.h"
#include "shaders.h"
#include "texture.h"

#include <glad/glad.h>

using namespace dagger;

class TextureSystem
	: public System
	, public Subscriber<AssetLoadRequest<Texture>>
{
	Sequence<UInt64> m_TextureHandles;

public:
	inline String SystemName() const override
	{
		return "Texture System";
	}

	static ViewPtr<Texture> Get(String name_);

	void OnLoadAsset(AssetLoadRequest<Texture> request_);
	void SpinUp() override;
	void WindDown() override;
};