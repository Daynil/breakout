#include "player.h"

#include <glm/glm.hpp>

void Player::Move(float dt, int level_width, float move)
{
	if (position.x >= 0 && position.x <= (level_width - size.x))
		position.x += velocity * dt * move;

	position.x = glm::clamp(position.x, 0.0f, level_width - size.x);
}
