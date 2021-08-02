#pragma once

#include "core/core.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/system.h"
#include "gameplay/common/simple_collisions.h"

#include <functional>

template<typename T>
JSON::json SerializeComponent(T& input_)
{
	assert(("Serialization failed: no type specialization found!", false)); // NOLINT
	return JSON::json {};
}

template<typename T>
void DeserializeComponent(JSON::json input_, T& fill_)
{
	assert(("Deserialization failed: no type specialization found!", false)); // NOLINT
}

// Serialize vectors

template<>
JSON::json SerializeComponent(Vector2& input_);

template<>
void DeserializeComponent(JSON::json input_, Vector2& fill_);

template<>
JSON::json SerializeComponent(Vector3& input_);

template<>
void DeserializeComponent(JSON::json input_, Vector3& fill_);

template<>
JSON::json SerializeComponent(Vector4& input_);

template<>
void DeserializeComponent(JSON::json input_, Vector4& fill_);

// Serialize sprites

template<>
JSON::json SerializeComponent(Sprite& input_);

template<>
void DeserializeComponent(JSON::json input_, Sprite& fill_);

// Serialize transforms

template<>
JSON::json SerializeComponent(Transform& input_);

template<>
void DeserializeComponent(JSON::json input_, Transform& fill_);

// Serialize animations

template<>
JSON::json SerializeComponent(Animator& input_);

template<>
void DeserializeComponent(JSON::json input_, Animator& fill_);

// Serialize collisions

template<>
JSON::json SerializeComponent(SimpleCollision& input_);

template<>
void DeserializeComponent(JSON::json input_, SimpleCollision& fill_);

// Serialize camera

template<>
JSON::json SerializeComponent(Camera& input_);

template<>
void DeserializeComponent(JSON::json input_, Camera& fill_);