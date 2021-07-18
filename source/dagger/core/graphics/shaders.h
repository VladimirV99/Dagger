#pragma once

#include "core/core.h"
#include "core/filesystem.h"
#include "core/system.h"
#include "shader.h"

#include <glad/glad.h>

#include <vector>

using namespace dagger;

class ShaderChangeRequest
{
	ViewPtr<Shader> m_Shader;

public:
	explicit ShaderChangeRequest(ViewPtr<Shader> shader_) : m_Shader {shader_} { }

	ViewPtr<Shader> GetShader()
	{
		return m_Shader;
	};
};

class ShaderSystem
	: public System
	, public Subscriber<AssetLoadRequest<Shader>>
{
public:
	inline String SystemName() const override
	{
		return "Shader System";
	}

	static void Use(String name_);
	static ViewPtr<Shader> Get(String name_);
	static UInt32 GetId(String name_);

	void OnLoadAsset(AssetLoadRequest<Shader> request_);
	void SpinUp() override;
	void WindDown() override;
};
