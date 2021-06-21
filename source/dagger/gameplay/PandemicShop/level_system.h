#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/system.h"
#include "gameplay/PandemicShop/level.h"

using namespace dagger;

namespace pandemic {

struct LoadedData
{
	String name;
	unsigned mapWidth;
	unsigned mapHeight;
	Sequence<Sequence<int>> tilemap;
};

class LevelSystem : public System
{
public:
	String SystemName() override {
		return "Level Sys";
	}

	void LoadAssets();
	void OnAssetLoadRequest(AssetLoadRequest<LoadedData> request_);

	void SpinUp() override;
	void Run() override;
	void WindDown() override;
};

}