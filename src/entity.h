#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "model.h"

class Entity
{
public:
	Model* model;

	glm::vec3 rotation;

	glm::vec3 position;
	float rotationX, rotationY, rotationZ;
	float scale;

	Entity(Model* pModel, glm::vec3 pPosition, float pRotationX, float pRotationY, float pRotationZ, float pScale);

private:
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};