#pragma once

#include <map>
#include <string>
#include <vector>

#include "texture.h"
#include "model.h"
#include "entity.h"
#include "shader_s.h"
#include "raw_model.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	GameState State = GAME_ACTIVE;

	std::map<std::string, RawModel> RawModels;
	std::map<std::string, Shader> Shaders;
	std::map<std::string, Texture> Textures;
	std::map<std::string, Model> Models;

	std::vector<Entity> Bricks;

	int LevelWidth;
	int LevelHeight;

	float left_stick_x;
	bool keyboard_keys[1024] = { false };
	bool gamepad_keys[1024] = { false };

	Game(int levelWidth, int levelHeight) : LevelWidth(levelWidth), LevelHeight(levelHeight) {};

	void Init();

	void LoadLevel(int level);

	// Game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
};