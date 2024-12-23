#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"

class Brick : public Entity
{
public:
	glm::vec4 color;
	int life;

	Brick(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 color = glm::vec4(0), int life = 1) : Entity(model, texture, position, rotation, scale), color(color), life(life) {};

	void ProvideRenderData(Shader& shader) override;
};