#include "audio.h"

#include <core/core.h>
#include <core/engine.h>

using namespace dagger;

void Audio::Initialize()
{
	m_SoLoud.init();
}

void Audio::Load(AssetLoadRequest<Sound> request_)
{
	Logger::info("Loading sound {}...", request_.path);

	FilePath path {request_.path};
	String name = path.stem().string();
	FilePath root {request_.path};
	root.remove_filename();

	String soundName = "";
	{
		String pathName = root.append(name).string();
		if (pathName.find("sounds") == 0)
			pathName = pathName.substr(7);

		std::replace(pathName.begin(), pathName.end(), '/', ':');
		std::replace(pathName.begin(), pathName.end(), '\\', ':');
		soundName = pathName;
	}

	auto& sounds = Engine::Res<Sound>();

	if (!sounds.contains(soundName))
		sounds[soundName] = new Sound();

	auto* sound = sounds[soundName];
	sound->name = soundName;
	sound->path = request_.path;
	sound->source.load(request_.path.c_str());
}

unsigned Audio::Play(String name_, float volume_)
{
	auto& sounds = Engine::Res<Sound>();
	assert(sounds.contains(name_));

	SoLoud::handle handle = m_SoLoud.play(sounds[name_]->source);
	m_SoLoud.setVolume(handle, volume_);

	return handle;
}

unsigned Audio::PlayLoop(String name_, float volume_)
{
	auto& sounds = Engine::Res<Sound>();
	assert(sounds.contains(name_));

	SoLoud::handle handle = m_SoLoud.play(sounds[name_]->source);
	m_SoLoud.setLooping(handle, true);
	m_SoLoud.setVolume(handle, volume_);

	return handle;
}

void Audio::Pause(unsigned handle_, bool pause_)
{
	m_SoLoud.setPause(handle_, pause_);
}

void Audio::Unpause(unsigned handle_)
{
	m_SoLoud.setPause(handle_, false);
}

void Audio::Stop(unsigned handle_)
{
	m_SoLoud.stop(handle_);
}

void Audio::StopAll()
{
	m_SoLoud.stopAll();
}

void Audio::Uninitialize()
{
	m_SoLoud.deinit();

	for (auto sound : Engine::Res<Sound>())
	{
		delete sound.second;
	}

	Engine::Res<Sound>().clear();
}

void AudioSystem::OnLoadAsset(AssetLoadRequest<Sound> request_)
{
	Engine::GetDefaultResource<Audio>()->Load(request_);
}

void AudioSystem::SpinUp()
{
	Engine::PutDefaultResource<Audio>(new Audio());
	auto* audio = Engine::GetDefaultResource<Audio>();
	audio->Initialize();

	Engine::Dispatcher().sink<AssetLoadRequest<Sound>>().connect<&AudioSystem::OnLoadAsset>(this);

	if (Files::exists("sounds") && Files::is_directory("sounds"))
	{
		for (auto& entry : Files::recursive_directory_iterator("sounds"))
		{
			if (entry.path().extension() == ".wav")
				Engine::Dispatcher().trigger<AssetLoadRequest<Sound>>(AssetLoadRequest<Sound> {entry.path().string()});
		}
	}
}

void AudioSystem::WindDown()
{
	auto* audio = Engine::GetDefaultResource<Audio>();
	audio->Uninitialize();
	delete audio;
}