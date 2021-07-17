#pragma once

#include "texture.h"
#include "shaders.h"
#include "core/system.h"
#include "core/core.h"

#include <glad/glad.h>

using namespace dagger;

class TextureSystem 
    : public System
    , public Subscriber<AssetLoadRequest<Texture>>
{
    Sequence<UInt64> m_TextureHandles;

public:
    inline String SystemName() override { return "Texture System"; }

    static ViewPtr<Texture> Get(String name_);

    void OnLoadAsset(AssetLoadRequest<Texture> request_);
    void SpinUp() override;
    void WindDown() override;
};