#include "level_system.h"

using namespace dagger;
using namespace pandemic;

void LevelSystem::SpinUp()
{
	Engine::Dispatcher().sink<AssetLoadRequest<LoadedData>>().connect<&LevelSystem::OnAssetLoadRequest>(this);

	LoadAssets();
};

void LevelSystem::LoadAssets()
{
	for (auto& entry : Files::recursive_directory_iterator("levels"))
	{
		auto path = entry.path().string();
		if (entry.is_regular_file() && entry.path().extension() == ".json")
		{
			Engine::Dispatcher().trigger<AssetLoadRequest<LoadedData>>(AssetLoadRequest<LoadedData>{ path });
		}
	}
}

void LevelSystem::OnAssetLoadRequest(AssetLoadRequest<LoadedData> request_)
{
	FilePath path(request_.path);
	Logger::info("Loading '{}'", request_.path);

	if (!Files::exists(path))
	{
		Engine::Dispatcher().trigger<Error>(Error{ fmt::format("Couldn't load map from {}.", request_.path) });
		return;
	}

	FileInputStream handle;
	auto absolutePath = Files::absolute(path);
	handle.open(absolutePath);

	if (!handle.is_open())
	{
		Engine::Dispatcher().trigger<Error>(Error{ fmt::format("Couldn't open map file '{}' for reading.", absolutePath.string()) });
		return;
	}

	JSON::json json;
	handle >> json;

	LoadedData* levelData = new LoadedData();
	assert(json.contains("name"));
	assert(json.contains("map_width"));
	assert(json.contains("map_height"));
	assert(json.contains("tilemap"));

	levelData->name = json["name"];
	levelData->mapWidth = json["map_width"];
	levelData->mapHeight = json["map_height"];


	// Tilemap
	Sequence<Sequence<int>> tiles;
	assert(json["tilemap"].size() == levelData->mapHeight);
	for (auto row = json["tilemap"].rbegin(); row != json["tilemap"].rend(); row++)
	{
		assert(row->size() == levelData->mapWidth);
		tiles.emplace_back();
		for (auto& tile : (*row)) {
			tiles.back().push_back(tile.get<int>()-1);
		}
	}
	levelData->tilemap = std::move(tiles);

	auto& library = Engine::Res<LoadedData>();
	if (library.contains(levelData->name))
	{
		delete library[levelData->name];
	}

	library[levelData->name] = levelData;
	Logger::info("Map '{}' loaded!", levelData->name);
}

void LevelSystem::Run()
{
}

void LevelSystem::WindDown()
{
	Engine::Dispatcher().sink<AssetLoadRequest<LoadedData>>().disconnect<&LevelSystem::OnAssetLoadRequest>(this);
};