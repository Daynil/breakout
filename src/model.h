#pragma once
#include <string>
#include <vector>

#include "texture.h"

class Model
{
public:
	unsigned int VAO_ID;
	Texture texture;
	unsigned int vertex_count;

	Model(std::string texturePath, bool textureAlpha, const std::vector<float>& vertex_positions, const std::vector<float>& vertex_texture_uvs, const std::vector<unsigned int >& vertex_indices);

private:
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};