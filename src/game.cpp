#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "resource_manager.h"

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

	ResourceManager::LoadRawModel("quad", RawModel(vertices, textureCoords, indices));

	ResourceManager::LoadTexture("ball", Texture(RESOURCES_PATH "awesomeface.png", true));
	ResourceManager::LoadTexture("wood", Texture(RESOURCES_PATH "container.jpg", false));
	ResourceManager::LoadTexture("brick", Texture(RESOURCES_PATH "brick.png", true));
	ResourceManager::LoadTexture("metal", Texture(RESOURCES_PATH "metal.png", true));

	ResourceManager::LoadShader("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));

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

	RawModel* model = &ResourceManager::GetRawModel("quad");

	int row = 0;
	while (std::getline(level_stream, line))
	{
		int col = 0;
		for (auto& brick_type : line)
		{
			Texture* texture;
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
				texture = &ResourceManager::GetTexture("metal");
			}
			else if (brick_type == '2') {
				life = 1;
				texture = &ResourceManager::GetTexture("wood");
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

				texture = &ResourceManager::GetTexture("brick");
			}

			Bricks.push_back(
				Brick(
					Entity(model, texture, glm::vec3(BRICK_WIDTH * col, BRICK_HEIGHT * row, 0), glm::vec3(0), glm::vec3(BRICK_WIDTH, BRICK_HEIGHT, 0), color),
					life
				)
			);

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
		brick.entity.position.x += (bricks_shift_x * BRICK_WIDTH);
		brick.entity.position.y += (bricks_shift_y * BRICK_HEIGHT);
	}
}

void Game::ProcessInput(float dt)
{
	if (State != GAME_ACTIVE)
		return;

	if (keyboard_keys[GLFW_KEY_A]) {
		std::cout << "pressed a" << std::endl;
	}
}

void Game::Update(float dt)
{
}

void Game::Render()
{
	renderer->prepare();

	for (auto& brick : Bricks)
	{
		renderer->render(brick.entity, ResourceManager::GetShader("entity"));
	}
}
