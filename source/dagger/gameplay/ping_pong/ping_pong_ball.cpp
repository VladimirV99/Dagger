#include "ping_pong_ball.h"

#include "core/engine.h"

using namespace dagger;
using namespace ping_pong;

void PingPongBallSystem::ResolveCollision(
	Transform& t_, SimpleCollision& col_, PingPongBall& ball_, const Transform& otherTransform_,
	const SimpleCollision& otherCollision_)
{
	Vector2 collisionSides = col_.GetCollisionSides(t_.position, otherCollision_, otherTransform_.position);
	do
	{
		// Get back for 1 frame
		Float32 dt = Engine::DeltaTime();
		if (std::abs(collisionSides.x) > 0)
		{
			t_.position.x -= (ball_.speed.x * dt);
		}

		if (std::abs(collisionSides.y) > 0)
		{
			t_.position.y -= (ball_.speed.y * dt);
		}

	} while (col_.IsCollided(t_.position, otherCollision_, otherTransform_.position));

	if (std::abs(collisionSides.x) > 0)
	{
		ball_.speed.x *= -1;
	}

	if (std::abs(collisionSides.y) > 0)
	{
		ball_.speed.y *= -1;
	}
}

void PingPongBallSystem::Run()
{
	auto viewCollisions = Engine::Registry().view<Transform, SimpleCollision>();
	auto view = Engine::Registry().view<PingPongBall, Transform, SimpleCollision>();
	for (auto entity : view)
	{
		auto& t = view.get<Transform>(entity);
		auto& ball = view.get<PingPongBall>(entity);
		auto& col = view.get<SimpleCollision>(entity);

		if (ball.reachedGoal)
		{
			continue;
		}

		if (col.colided && Engine::Registry().valid(col.colidedWith))
		{
			SimpleCollision& collision = viewCollisions.get<SimpleCollision>(col.colidedWith);
			Transform& transform = viewCollisions.get<Transform>(col.colidedWith);

			ResolveCollision(t, col, ball, transform, collision);

			if (Engine::Registry().all_of<PingPongWall>(col.colidedWith))
			{
				PingPongWall& wall = Engine::Registry().get<PingPongWall>(col.colidedWith);
				ball.reachedGoal = true;
				ball.playerOneScored = !wall.isLeft;
			}

			col.colided = false;
		}
		else
		{
			t.position += (ball.speed * Engine::DeltaTime());
		}
	}
}
