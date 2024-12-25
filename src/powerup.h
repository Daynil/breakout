#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"

class Powerup : public Entity
{
public:
	glm::vec4 color;
	std::string type;

	float velocity = 100.0f;

	Powerup(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 color = glm::vec4(0), std::string type = "") : Entity(model, texture, position, rotation, scale), color(color), type(type) {};

	void ProvideRenderData(Shader& shader) override
	{
		shader.setVec4("u_color", color);
	};

	bool Move(float dt, int level_height)
	{
		position.y += velocity * dt;

		if (position.y >= level_height) {
			return true;
		}

		return false;
	}
};