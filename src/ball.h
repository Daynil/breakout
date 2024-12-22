#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "player.h"

class Ball : public Entity
{
public:
	Player* player;

	float initial_velocity_x = 100.0f;
	float initial_velocity_y = 350.0f;

	bool stuck = true;
	glm::vec3 velocity = glm::vec3(initial_velocity_x, -initial_velocity_y, 0.0f);

	Ball(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Player* player, std::optional<glm::vec4> p_rgba_color = std::nullopt) : Entity(model, texture, position, rotation, scale, p_rgba_color), player(player) {};

	void Release();

	void Reset();

	void Move(float dt, int level_width, int level_height, bool should_release, float player_movement);
};