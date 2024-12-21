#include "ball.h"

void Ball::Release()
{
	stuck = false;
}

void Ball::Reset()
{
	stuck = true;
}

void Ball::Move(float dt, int level_width)
{
	if (stuck)
		return;

	position += velocity * dt;

	if (position.x <= 0.0f) {
		velocity.x = -velocity.x;
		position.x = 0.0f;
	}
	else if ((position.x + scale.x) >= level_width) {
		velocity.x = -velocity.x;
		position.x = level_width - scale.x;
	}

	if (position.y <= 0.0f) {
		velocity.y = -velocity.y;
		position.y = 0.0f;
	}
}
