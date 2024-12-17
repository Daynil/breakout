#include <map>
#include <string>
#include <vector>

#include "texture.h"
#include "model.h"
#include "entity.h"
#include "shader_s.h"

class Game 
{
public:
	std::map<std::string, Shader> Shaders;
	std::map<std::string, Texture> Textures;
	std::map<std::string, Model> Models;

	std::vector<Entity> Bricks;

	Game() {};

	void Init();

	void LoadLevel();
};