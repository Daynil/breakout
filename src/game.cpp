#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

int BRICK_WIDTH = 50;
int BRICK_HEIGHT = 25;

void Game::Init()
{
	// Our quad for all objects
	const std::vector<float> vertices = {
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
	};

	const std::vector<float> textureCoords = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
	};

	const std::vector<unsigned int> indices = {
		0, 2, 1,
		0, 1, 3
	};

	RawModels.emplace("quad", RawModel(vertices, textureCoords, indices));

	Textures.emplace("ball", Texture(RESOURCES_PATH "awesomeface.png", true));
	Textures.emplace("wood", Texture(RESOURCES_PATH "container.jpg", false));
	Textures.emplace("brick", Texture(RESOURCES_PATH "brick.png", true));
	Textures.emplace("metal", Texture(RESOURCES_PATH "metal.png", true));

	Models.emplace("ball", Model(&RawModels.at("quad"), &Textures.at("ball")));
	Models.emplace("wood", Model(&RawModels.at("quad"), &Textures.at("wood")));
	Models.emplace("brick", Model(&RawModels.at("quad"), &Textures.at("brick")));
	Models.emplace("metal", Model(&RawModels.at("quad"), &Textures.at("metal")));

	Shaders.emplace("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));

	LoadLevel(3);
}

void Game::LoadLevel(int level)
{
	int max_bricks_x = LevelWidth / BRICK_WIDTH;
	int max_bricks_y = LevelHeight / BRICK_HEIGHT * 0.75;

	std::ifstream level_stream(RESOURCES_PATH "level" + std::to_string(level) + ".txt");
	std::string line;
	std::stringstream ss;

	int longest_row = 0;
	int tallest_col = 0;

	int row = 0;
	while (std::getline(level_stream, line))
	{
		int col = 0;
		for (auto& brick_type : line)
		{
			Model* model;
			int life = 0;
			std::optional<glm::vec4> color = std::nullopt;
			if (brick_type == '0') {
				col += 1;
				if (col > longest_row)
					longest_row = col;
				continue;
			}
			else if (brick_type == '1') {
				life = 999;
				model = &Models.at("metal");
			}
			else if (brick_type == '2') {
				life = 1;
				model = &Models.at("wood");
			}
			else {
				int brick_numeric = std::stoi(std::string(1, brick_type));
				life = brick_numeric - 1;
				// Regular brick no color for 2
				if (brick_numeric == 3)
					color = glm::vec4(0, 0.5, 0, 0.55);
				if (brick_numeric == 4)
					color = glm::vec4(0, 0, 0.5, 0.55);
				if (brick_numeric == 5)
					color = glm::vec4(0.5, 0, 0, 0.55);

				model = &Models.at("brick");
			}

			// TODO: make new model for brick which has an entity and "Life"

			Bricks.push_back(Entity(model, glm::vec3(BRICK_WIDTH * col, BRICK_HEIGHT * row, 0), glm::vec3(0), glm::vec3(BRICK_WIDTH, BRICK_HEIGHT, 0), color));

			col += 1;
			if (col > longest_row)
				longest_row = col;
		}
		row += 1;
		if (row > tallest_col)
			tallest_col = row;
	}

	int bricks_shift_x = (max_bricks_x - longest_row) / 2;
	int bricks_shift_y = (max_bricks_y - tallest_col) / 2;

	for (auto& brick : Bricks)
	{
		brick.position.x += (bricks_shift_x * BRICK_WIDTH);
		brick.position.y += (bricks_shift_y * BRICK_HEIGHT);
	}
}
