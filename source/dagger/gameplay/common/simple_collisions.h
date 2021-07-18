#pragma once
#include "core/core.h"
#include "core/system.h"

using namespace dagger;

struct SimpleCollision
{
	Vector2 size;
	Vector2 pivot {-0.5f, -0.5f};

	bool colided = false;
	entt::entity colidedWith;

	bool IsCollided(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_) const;

	// return (0,1) if collision happen by y, (1,0) if collision happen by x
	Vector2 GetCollisionSides(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_) const;

	Vector3 GetCollisionCenter(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_) const;
};

class SimpleCollisionsSystem : public System
{
public:
	inline String SystemName() const override
	{
		return "Simple Collisions System";
	}

	void Run() override;
};
