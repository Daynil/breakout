#include <map>
#include <string>
#include <vector>

#include "texture.h"
#include "model.h"
#include "entity.h"
#include "shader_s.h"
#include "raw_model.h"

class Game
{
public:
	std::map<std::string, RawModel> RawModels;
	std::map<std::string, Shader> Shaders;
	std::map<std::string, Texture> Textures;
	std::map<std::string, Model> Models;

	std::vector<Entity> Bricks;

	int LevelWidth;
	int LevelHeight;

	Game(int levelWidth, int levelHeight) : LevelWidth(levelWidth), LevelHeight(levelHeight) {};

	void Init();

	void LoadLevel(int level);
};