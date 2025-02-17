#pragma once

#include "core/core.h"
#include "core/graphics/shader.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/system.h"

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

using namespace dagger;

class ToolRenderSystem
	: public System
	, public Subscriber<Render, ShaderChangeRequest>
{
	const Float32 m_VerticesAndTexCoords[24] = {-0.5f, -0.5f, 0.0f, 0.0f, -0.5f, 0.5f,	0.0f, 1.0f,
												0.5f,  0.5f,  1.0f, 1.0f, 0.5f,	 -0.5f, 1.0f, 0.0f,
												-0.5f, -0.5f, 0.0f, 0.0f, 0.5f,	 0.5f,	1.0f, 1.0f};

	UInt32 m_VAO;
	UInt32 m_StaticMeshVBO;
	UInt32 m_InstanceQuadInfoVBO;
	Float32* m_Data;

	UInt8 m_Index = 0;

	void OnRender();

public:
	inline String SystemName() const override
	{
		return "Tool Render System";
	}

	constexpr static UInt64 s_VertexCount = 24;
	constexpr static UInt64 s_SizeOfMesh = sizeof(Float32) * s_VertexCount;
	constexpr static UInt64 s_MaxNumberOfMeshes = 10000;
	constexpr static UInt64 s_BufferSize = sizeof(Float32) * s_VertexCount * s_MaxNumberOfMeshes;

	void SpinUp() override;
	void WindDown() override;

	Registry* registry;
};