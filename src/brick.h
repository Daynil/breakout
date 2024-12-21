#pragma once

#include "entity.h"

class Brick : public Entity
{
public:
	int life;

	Brick(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::optional<glm::vec4> p_rgba_color = std::nullopt, int life = 1) : Entity(model, texture, position, rotation, scale, p_rgba_color), life(life) {};
};