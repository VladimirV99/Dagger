#pragma once

#include "camera.h"
#include "core/core.h"
#include "core/system.h"
#include "shaders.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <deque>
#include <sstream>

using namespace dagger;

struct PreRender
{
};
struct Render
{
};
struct ToolRender
{
};
struct PostRender
{
};
struct GUIRender
{
};

struct GLFWwindow;

struct CameraUpdate EMPTY_EVENT
{
};

struct WindowResized
{
	GLFWwindow* window;
	UInt32 width;
	UInt32 height;
};

struct RenderConfig
{
	Bool fullscreen;
	Bool resizable;
	Bool vsync;
	GLsizei windowWidth;
	GLsizei windowHeight;
	GLFWwindow* window;
	Matrix4 projection;
	Matrix4 viewport;
	Vector2 viewOffset;
	Vector2 lastSize;
	Vector4 viewBounds;
	Matrix4 camera;
};

struct CachedMatrices
{
	GLuint viewportMatrixId;
	GLuint projectionMatrixId;
	GLuint cameraMatrixId;
};

class WindowSystem
	: public System
	, public Publisher<PreRender, Render, ToolRender, KeyboardEvent, MouseEvent, CursorEvent, Error>
{
	RenderConfig m_Config;
	CachedMatrices m_Matrices;
	Camera m_Camera;

public:
	WindowSystem() : m_Config {}, m_Matrices {}, m_Camera {} { }

	WindowSystem(const WindowSystem&) = delete;

	inline String SystemName() const override
	{
		return "Window System";
	}

	void UpdateViewProjectionMatrix();

	void UpdateViewProjectionMatrix(RenderConfig& config_, Camera& camera_);
	void SetViewProjectionMatrix(RenderConfig& config_, Camera& camera_, Float32 width_, Float32 height_);

	void UpdateCameraMatrix();

	void OnWindowResized(WindowResized resized_);
	void OnShaderChanged(ShaderChangeRequest request_);

	void SpinUp() override;
	void Run() override;
	void WindDown() override;
};