#pragma once

#include <map>
#include <string>

#include "shader_s.h"
#include "raw_model.h"
#include "texture.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, RawModel> RawModels;
	static std::map<std::string, Texture> Textures;

	static Shader& LoadShader(std::string name, Shader shader);
	static Shader& GetShader(std::string name);

	static Texture& LoadTexture(std::string name, Texture texture);
	static Texture& GetTexture(std::string name);

	static RawModel& LoadRawModel(std::string name, RawModel model);
	static RawModel& GetRawModel(std::string name);

	// Properly deallocate all loaded resources
	static void Clear();
private:
	ResourceManager() {};
};