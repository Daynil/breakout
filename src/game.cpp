#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

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

	ResourceManager::LoadTexture("background", Texture(RESOURCES_PATH "background.jpg", false));
	ResourceManager::LoadTexture("paddle", Texture(RESOURCES_PATH "paddle.png", true));
	ResourceManager::LoadTexture("ball", Texture(RESOURCES_PATH "ball.png", true));
	ResourceManager::LoadTexture("wood", Texture(RESOURCES_PATH "container.jpg", false));
	ResourceManager::LoadTexture("brick", Texture(RESOURCES_PATH "brick.png", true));
	ResourceManager::LoadTexture("metal", Texture(RESOURCES_PATH "metal.png", true));

	ResourceManager::LoadShader("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));

	LoadLevel(3);

	glm::vec3 player_size = glm::vec3(100.0f, 20.0f, 0.0f);
	glm::vec3 playerPos = glm::vec3(
		LevelWidth / 2.0f - player_size.x / 2.0f,
		LevelHeight - player_size.y, 0.0f
	);
	player = new Player(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("paddle"),
		playerPos,
		glm::vec3(0),
		player_size
	);

	glm::vec3 initial_ball_velocity = glm::vec3(100.0f, -350.0f, 0.0f);
	float ball_radius = 10.0f;
	ball = new Ball(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("ball"),
		glm::vec3(0),
		glm::vec3(0),
		glm::vec3(ball_radius * 2, ball_radius * 2, 0.0f),
		player
	);
	ball->Reset();
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
					model, texture, glm::vec3(BRICK_WIDTH * col, BRICK_HEIGHT * row, 0), glm::vec3(0), glm::vec3(BRICK_WIDTH, BRICK_HEIGHT, 0), color,
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
		brick.position.x += (bricks_shift_x * BRICK_WIDTH);
		brick.position.y += (bricks_shift_y * BRICK_HEIGHT);
	}
}

void Game::ProcessInput(float dt)
{
	if (State != GAME_ACTIVE)
		return;

	float player_movement = 0;
	float should_release = false;

	// Keyboard controls
	if (keyboard_keys[GLFW_KEY_A] || keyboard_keys[GLFW_KEY_LEFT])
		player_movement = -1;
	if (keyboard_keys[GLFW_KEY_D] || keyboard_keys[GLFW_KEY_RIGHT])
		player_movement = 1;
	if (keyboard_keys[GLFW_KEY_SPACE])
		should_release = true;

	// Gamepad controls
	if (left_stick_x != 0)
		player_movement = left_stick_x;

	if (gamepad_keys[GLFW_GAMEPAD_BUTTON_X])
		should_release = true;

	player->Move(dt, LevelWidth, player_movement);
	ball->Move(dt, LevelWidth, LevelHeight, should_release, player_movement);
}

void Game::Update(float dt)
{
	CheckCollisions();
}

void Game::Render()
{
	renderer->prepare();

	renderer->render(Entity(&ResourceManager::GetRawModel("quad"), &ResourceManager::GetTexture("background"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(LevelWidth, LevelHeight, 0)), ResourceManager::GetShader("entity"));
	for (auto& brick : Bricks)
	{
		if (brick.life > 0)
			renderer->render(brick, ResourceManager::GetShader("entity"));
	}
	renderer->render(*player, ResourceManager::GetShader("entity"));
	renderer->render(*ball, ResourceManager::GetShader("entity"));
}

void Game::CheckCollisions()
{
	for (Brick& brick : Bricks) {
		if (brick.life > 0) {
			Collision collision(CheckCollision(*ball, brick));
			if (collision.occured) {
				brick.life = 0;
				glm::vec2 direction(1.0f, 1.0f);
				if (collision.vertical)
					direction.y = -1.0f;
				else
					direction.x = -1.0f;
				ball->Rebound(direction);
			}
		}
	}

	Collision collision(CheckCollision(*ball, *player));
	if (collision.occured) {
		glm::vec2 direction(1.0f, 1.0f);
		glm::vec2 recoil(0.0f, 0.0f);
		if (collision.vertical) {
			float player_center = player->position.x + (player->scale.x / 2);
			float collision_intensity = (collision.coord.x - player_center) / (player->scale.x / 2.0f);
			direction.y = -1.0f;
			direction.x = collision_intensity < 0 ? -1.0f : 1.0f;
			recoil.x = collision_intensity * 10.0f;
			//direction.x = glm::clamp(collision_dist, -1.5f, 1.5f);
		}
		else
			direction.x = -1.0f;
		ball->Rebound(direction, recoil);
	}
}

Collision Game::CheckCollision(Ball& ball, Entity& two)
{
	// Simple collision check if both are square colliders
	//bool x_overlap = (one.position.x + one.scale.x) >= two.position.x && one.position.x <= (two.position.x + two.scale.x);
	//bool y_overlap = (one.position.y + one.scale.y) >= two.position.y && one.position.y <= (two.position.y + two.scale.y);
	//return x_overlap && y_overlap;

	float ball_radius = ball.scale.x / 2.0f;

	glm::vec2 center(ball.position + ball_radius);
	glm::vec2 aabb_half_extents(two.scale.x / 2.0f, two.scale.y / 2.0f);
	glm::vec2 aabb_center(
		two.position.x + aabb_half_extents.x,
		two.position.y + aabb_half_extents.y
	);

	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	bool collision = glm::length(difference) < ball_radius;
	bool collision_vertical = false;
	if (collision) {
		if (closest.y <= two.position.y || closest.y >= (two.position.y + two.scale.y)) {
			collision_vertical = true;
			//std::cout << "vertical!" << std::endl;
		}
		else {
			//std::cout << "horizontal!" << std::endl;
		}
	}

	return Collision{ collision, collision_vertical, closest };
}

Game::~Game()
{
	delete player;
}