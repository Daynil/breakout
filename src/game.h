#pragma once

#include <map>
#include <string>
#include <vector>

#include "texture.h"
#include "entity.h"
#include "shader_s.h"
#include "raw_model.h"
#include "brick.h"
#include "renderer.h"
#include "player.h"
#include "ball.h"
#include "particle_manager.h"
#include "text_renderer.h"
#include "powerup.h"
#include "laser.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct Collision {
	bool occured;
	Direction dir;
	glm::vec2 diff;
};

class Game
{
public:
	GameState State = GAME_MENU;

	Renderer* renderer;
	TextRenderer* text_renderer;

	std::vector<Brick> Bricks;
	std::vector<Powerup> powerups;

	Player* player;
	Ball* ball;

	LaserGun* laser_gun = nullptr;

	ParticleManager* particle_manager;

	int LevelWidth;
	int LevelHeight;

	int lives = 3;
	int level = 1;

	float left_stick_x = 0;
	bool keyboard_keys[1024] = { false };
	bool keyboard_keys_processed[1024] = { false };
	bool gamepad_keys[1024] = { false };
	bool gamepad_keys_processed[1024] = { false };

	Game(int levelWidth, int levelHeight, Renderer* renderer) : LevelWidth(levelWidth), LevelHeight(levelHeight), renderer(renderer) {};
	~Game();

	void Init();

	bool LevelComplete();
	void ResetGame();
	void LoadLevel(int level);

	void RollForPowerup(Brick& brick_destroyed);

	// Game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void CheckCollisions();
	Collision CheckBallCollision(Ball& one, Entity& two);
	Collision CheckCollision(Entity& one, Entity& two);
};