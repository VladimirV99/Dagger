#include "textures.h"

#include "core/engine.h"
#include "core/filesystem.h"
#include "core/graphics/sprite.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

using namespace dagger;

ViewPtr<Texture> TextureSystem::Get(String name_)
{
	auto* texture = Engine::Res<Texture>()[name_];
	assert(texture != nullptr);
	return texture;
}

void TextureSystem::OnLoadAsset(AssetLoadRequest<Texture> request_)
{
	Logger::info("Loading texture {}...", request_.path);
	stbi_set_flip_vertically_on_load(1);

	FilePath path {request_.path};
	FilePath root {request_.path};
	root.remove_filename();

	String textureName;
	{
		String pathName = root.append(path.stem().string()).string();
		if (pathName.find("textures") == 0)
			pathName = pathName.substr(9, pathName.length() - 9);

		std::replace(pathName.begin(), pathName.end(), '/', ':');
		std::replace(pathName.begin(), pathName.end(), '\\', ':');
		textureName = pathName;
	}

	// these have to remain "int" because of API/ABI-compatibility with stbi_load
	int width, height, channels;
	UInt8* image = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
	if (image == nullptr)
	{
		Engine::Dispatcher().trigger<Error>(Error {fmt::format("Failed to load texture: {}", path.string())});
		return;
	}

	Logger::info(
		"Image statistics: name ({}), width ({}), height ({}), depth ({})", textureName, width, height, channels);

	auto& textures = Engine::Res<Texture>();

	if (textures.contains(textureName))
		delete textures[textureName];

	auto* texture = new Texture(textureName, path, image, (UInt32)width, (UInt32)height, (UInt32)channels);

	assert(width != 0);
	texture->m_Ratio = (Float32)height / (Float32)width;
	assert(texture->m_Ratio != 0);

	textures[textureName] = texture;
	Logger::info("Texture saved under \"{}\"", textureName);
	stbi_image_free(image);
}

void TextureSystem::SpinUp()
{
	Engine::Dispatcher().sink<AssetLoadRequest<Texture>>().connect<&TextureSystem::OnLoadAsset>(this);

	for (const auto& entry : Files::recursive_directory_iterator("textures"))
	{
		if (entry.path().extension() == ".png")
			Engine::Dispatcher().trigger<AssetLoadRequest<Texture>>(AssetLoadRequest<Texture> {entry.path().string()});
	}

	Engine::Dispatcher().trigger<AssetLoadFinished<Texture>>(AssetLoadFinished<Texture> {});
}

void TextureSystem::WindDown()
{
	auto& textures = Engine::Res<Texture>();
	for (const auto& texture : textures)
	{
		delete texture.second;
	}

	textures.clear();

	Engine::Dispatcher().sink<AssetLoadRequest<Texture>>().disconnect<&TextureSystem::OnLoadAsset>(this);
}
