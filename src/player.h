#pragma once

#include <glm/glm.hpp>

#include "entity.h"

class Player
{
public:
	Entity entity;

	glm::vec2 size = glm::vec2(100.0f, 20.0f);
	float velocity = 500.0f;

	Player(Entity entity) : entity(entity) {};
};