#pragma once

#include <glm/glm.hpp>

#include "entity.h"

class Ball : public Entity
{
public:
	float radius;
	bool stuck = true;
	glm::vec3 velocity = glm::vec3(100.0f, -350.0f, 0.0f);

	Ball(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		: Entity(model, texture, position, rotation, scale) {
	};

	void Release();

	void Reset();

	void Move(float dt, int level_width);
};