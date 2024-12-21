#pragma once

#include <glm/glm.hpp>

#include "entity.h"

class Player : public Entity
{
public:
	glm::vec2 size = glm::vec2(100.0f, 20.0f);
	float velocity = 500.0f;

	Player(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		: Entity(model, texture, position, rotation, scale) {
	};
};