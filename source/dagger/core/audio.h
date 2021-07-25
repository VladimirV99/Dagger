#pragma once

#include "core/core.h"
#include "core/system.h"

#include "soloud.h"
#include "soloud_wav.h"

#include <string>

struct Sound
{
	String name;
	String path;
	SoLoud::Wav source;
};

struct Audio
{
	void Initialize();
	void Load(AssetLoadRequest<Sound> request_);
	unsigned Play(String name_, float volume_ = 1.0f);
	unsigned PlayLoop(String name_, float volume_ = 1.0f);
	void Stop(unsigned handle_);
	void StopAll();
	void Uninitialize();
private:
	SoLoud::Soloud m_SoLoud;
};

struct AudioSystem : public dagger::System
{
	inline String SystemName() const override
	{
		return "AudioSystem";
	}

	void OnLoadAsset(AssetLoadRequest<Sound> request_);
	void SpinUp() override;
	void WindDown() override;
};