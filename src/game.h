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

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

struct Collision {
	bool occured = false;
	bool vertical = false;
	glm::vec2 coord;
};

//// Initial size of the player paddle
//const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
//// Initial velocity of the player paddle
//const float PLAYER_VELOCITY(500.0f);

class Game
{
public:
	GameState State = GAME_ACTIVE;

	Renderer* renderer;

	std::vector<Brick> Bricks;
	Player* player;
	Ball* ball;

	int LevelWidth;
	int LevelHeight;

	float left_stick_x = 0;
	bool keyboard_keys[1024] = { false };
	bool gamepad_keys[1024] = { false };

	Game(int levelWidth, int levelHeight, Renderer* renderer) : LevelWidth(levelWidth), LevelHeight(levelHeight), renderer(renderer) {};
	~Game();

	void Init();

	void LoadLevel(int level);

	// Game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void CheckCollisions();
	Collision CheckCollision(Ball& one, Entity& two);
};