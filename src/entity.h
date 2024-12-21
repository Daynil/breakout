#pragma once
#include <glm/glm.hpp>

#include <vector>
#include <optional>

#include "texture.h"
#include "raw_model.h"

class Entity
{
public:
	RawModel* model;
	Texture* texture;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::optional<glm::vec4> rgba_color;

	Entity() {};
	Entity(RawModel* pModel, Texture* texture, glm::vec3 pPosition, glm::vec3 pRotation, glm::vec3 pScale, std::optional<glm::vec4> p_rgba_color = std::nullopt);

	virtual ~Entity() {};

private:
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};