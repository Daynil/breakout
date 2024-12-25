#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <raudio.h>

#include "resource_manager.h"
#include "text_renderer.h"

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
	ResourceManager::LoadTexture("particle", Texture(RESOURCES_PATH "particle.png", true));

	ResourceManager::LoadShader("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));
	ResourceManager::LoadShader("entity_tinted", Shader(RESOURCES_PATH "shaders/entity_tinted.shader"));
	ResourceManager::LoadShader("particle", Shader(RESOURCES_PATH "shaders/particle.shader"));
	ResourceManager::LoadShader("text", Shader(RESOURCES_PATH "shaders/text.shader"));

	InitAudioDevice();
	//ResourceManager::LoadRSound("brick", RESOURCES_PATH "bleep.mp3");
	ResourceManager::LoadRSound("brick", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("brick_solid", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("powerup", RESOURCES_PATH "powerup.wav");
	ResourceManager::LoadRSound("paddle", RESOURCES_PATH "bleep.wav");

	text_renderer = new TextRenderer(LevelWidth, LevelHeight, RESOURCES_PATH "PressStart2P.ttf");

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

	particle_manager = new ParticleManager(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("particle"),
		&ResourceManager::GetShader("particle"),
		ball
	);
	particle_manager->Init(800);

	ResetGame();
}

void Game::ResetGame()
{
	ball->Reset();
	lives = 3;
	LoadLevel(level);
}

void Game::LoadLevel(int p_level)
{
	Bricks.clear();

	int max_bricks_x = LevelWidth / BRICK_WIDTH;
	int max_bricks_y = LevelHeight / BRICK_HEIGHT * 0.75;

	std::ifstream level_stream(RESOURCES_PATH "level" + std::to_string(p_level) + ".txt");
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
			glm::vec4 color = glm::vec4(0);
			bool solid = false;
			if (brick_type == '0') {
				col += 1;
				if (col > longest_row)
					longest_row = col;
				continue;
			}
			else if (brick_type == '1') {
				life = 999;
				texture = &ResourceManager::GetTexture("metal");
				solid = true;
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
					life, solid
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
	if (State == GAME_MENU) {
		if (keyboard_keys[GLFW_KEY_ENTER] && !keyboard_keys_processed[GLFW_KEY_ENTER]) {
			State = GAME_ACTIVE;
			keyboard_keys_processed[GLFW_KEY_ENTER] = true;
		}

		if (keyboard_keys[GLFW_KEY_W] && !keyboard_keys_processed[GLFW_KEY_W]) {
			level += 1;
			if (level == 4)
				level = 3;
			LoadLevel(level);
			keyboard_keys_processed[GLFW_KEY_W] = true;
		}
		if (keyboard_keys[GLFW_KEY_S] && !keyboard_keys_processed[GLFW_KEY_S]) {
			level -= 1;
			if (level == 0)
				level = 1;
			LoadLevel(level);
			keyboard_keys_processed[GLFW_KEY_S] = true;
		}
		return;
	}
	else if (State == GAME_WIN) {
		if (keyboard_keys[GLFW_KEY_ENTER] && !keyboard_keys_processed[GLFW_KEY_ENTER]) {
			State = GAME_MENU;
			keyboard_keys_processed[GLFW_KEY_ENTER] = true;
		}
		return;
	}

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
	bool life_lost = ball->Move(dt, LevelWidth, LevelHeight, should_release, player_movement);

	if (life_lost)
	{
		lives -= 1;
		if (lives == 0) {
			ResetGame();
			State = GAME_MENU;
		}
	}
}

void Game::Update(float dt)
{
	CheckCollisions();
	particle_manager->Update(dt);
	if (LevelComplete()) {
		ResetGame();
		State = GAME_WIN;
	}
}


bool Game::LevelComplete()
{
	for (Brick& brick : Bricks) {
		if (!brick.solid && brick.life > 0)
			return false;
	}
	return true;
}

void Game::Render()
{
	renderer->prepare();

	renderer->render(Entity(&ResourceManager::GetRawModel("quad"), &ResourceManager::GetTexture("background"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(LevelWidth, LevelHeight, 0)), ResourceManager::GetShader("entity"));
	for (auto& brick : Bricks)
	{
		if (brick.life > 0)
			renderer->render(brick, ResourceManager::GetShader("entity_tinted"));
	}
	renderer->render(*player, ResourceManager::GetShader("entity"));
	renderer->render(*ball, ResourceManager::GetShader("entity"));

	particle_manager->Render(*renderer);
	text_renderer->RenderText("Lives: " + std::to_string(lives), glm::vec2(5.0f, 5.0f), 0.4f);

	if (State == GAME_MENU) {
		text_renderer->RenderText("Press Enter or Start to start", glm::vec2(140.0f, LevelHeight / 2.0f), 0.4f);
		text_renderer->RenderText("Press W or S to select level", glm::vec2(210.0f, (LevelHeight / 2.0f) + 30.0f), 0.3f);
	}
	else if (State == GAME_WIN) {
		text_renderer->RenderText("You Won!!!", glm::vec2(240.0f, LevelHeight / 2.0f), 0.4f);
		text_renderer->RenderText("Press Enter to retry or Esc to quit", glm::vec2(190.0f, (LevelHeight / 2.0f) + 30.0f), 0.3f);
	}
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::CheckCollisions()
{
	for (Brick& brick : Bricks) {
		if (brick.life > 0) {
			Collision collision(CheckCollision(*ball, brick));
			if (collision.occured) {
				if (!brick.solid) {
					int orig_life = brick.life;
					brick.life -= 1;
					if (orig_life > 1 && brick.life == 1)
						brick.color = glm::vec4(0);
					if (brick.life == 2)
						brick.color = glm::vec4(0, 0.5, 0, 0.55);
					if (brick.life == 3)
						brick.color = glm::vec4(0, 0, 0.5, 0.55);
				}

				PlaySound(ResourceManager::GetSound("brick"));

				if (collision.dir == LEFT || collision.dir == RIGHT) {
					ball->velocity.x = -ball->velocity.x;
					float penetration = (ball->scale.x / 2.0f) - std::abs(collision.diff.x);
					if (collision.dir == LEFT)
						ball->position.x += penetration;
					else
						ball->position.x -= penetration;
				}
				else {
					ball->velocity.y = -ball->velocity.y;
					float penetration = (ball->scale.x / 2.0f) - std::abs(collision.diff.y);
					if (collision.dir == UP)
						ball->position.y -= penetration;
					else
						ball->position.y += penetration;
				}
			}
		}
	}

	if (!ball->stuck) {
		Collision collision(CheckCollision(*ball, *player));
		if (collision.occured) {
			PlaySound(ResourceManager::GetSound("paddle"));
			float center_board = player->position.x + player->scale.x / 2.0f;
			float dist = (ball->position.x + (ball->scale.x / 2.0f) - center_board);
			float percentage = dist / (player->scale.x / 2.0f);
			float strength = 2.0f;
			glm::vec2 old_velocity = ball->velocity;
			ball->velocity.x = ball->initial_velocity_x * percentage * strength;
			ball->velocity.y = -1.0f * std::abs(ball->velocity.y);
			ball->velocity = glm::normalize(ball->velocity) * glm::length(old_velocity);
		}
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
	Direction dir = VectorDirection(difference);

	return Collision{ collision, dir, difference };
}

Game::~Game()
{
	delete player;
	delete text_renderer;
	delete ball;
}