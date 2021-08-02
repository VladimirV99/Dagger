#include "camera.h"

#include "window.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

Vector2 Camera::WindowToScreen(Vector2 windowCoord_)
{
	auto* config = Engine::GetDefaultResource<RenderConfig>();
	auto* camera = Engine::GetDefaultResource<Camera>();

	Vector2 cursorInScreen {0, 0};

	const auto mat = config->projection * config->camera;
	auto prod = (mat * glm::vec4(windowCoord_, 0, 0)) / (2 * camera->zoom);
	cursorInScreen.x = prod.x;
	cursorInScreen.y = prod.y;

	return cursorInScreen;
}

Vector2 Camera::WindowToWorld(Vector2 windowCoord_)
{
	Vector2 cursorInWorld {0, 0};
	auto* config = Engine::GetDefaultResource<RenderConfig>();

	auto pos = glm::unProject(
		Vector3 {
			windowCoord_.x - config->viewOffset.x, config->windowHeight - windowCoord_.y - config->viewOffset.y, 0},
		config->camera, config->projection, config->viewBounds);
	cursorInWorld.x = pos.x;
	cursorInWorld.y = pos.y;
	return cursorInWorld;
}

Vector2 Camera::WorldToWindow(Vector2 worldCoord_)
{
	Vector2 cursorInWindow {0, 0};
	auto* config = Engine::GetDefaultResource<RenderConfig>();
	auto* camera = Engine::GetDefaultResource<Camera>();

	auto pos = glm::project(
		Vector3 {worldCoord_ + (camera->size / (2.0f * camera->zoom)), 0}, config->camera, config->projection,
		config->viewBounds);

	cursorInWindow.x = pos.x;
	cursorInWindow.y = config->windowHeight - pos.y;
	return cursorInWindow;
}
