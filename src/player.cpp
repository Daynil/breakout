#include "player.h"

#include <iostream>

#include <glm/glm.hpp>

void Player::CollectPowerup(std::string collected_powerup)
{
	std::cout << "Collected: " + collected_powerup << std::endl;

	for (auto& powerup : active_powerups) {
		if (powerup.powerup == collected_powerup) {
			powerup.seconds_left = 10.0f;
			return;
		}
	}

	active_powerups.push_back({ collected_powerup, 10.0f });
	if (collected_powerup == "speed") {
		velocity *= 2.0f;
		color = glm::vec4(0.9f, 0, 0, 0.3f);
	}
}

void Player::Move(float dt, int level_width, float move)
{
	if (position.x >= 0 && position.x <= (level_width - scale.x))
		position.x += velocity * dt * move;

	position.x = glm::clamp(position.x, 0.0f, level_width - scale.x);
}

void Player::Update(float dt)
{
	for (auto it = active_powerups.begin(); it != active_powerups.end();) {
		it->seconds_left -= dt;
		if (it->seconds_left <= 0) {
			if (it->powerup == "speed") {
				velocity = start_velocity;
			}
			std::cout << "Expired: " + it->powerup << std::endl;
			it = active_powerups.erase(it);
			color = glm::vec4(0);
		}
		else {
			++it;
		}
	}
}
