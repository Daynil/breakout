#include "resource_manager.h"

#include <glad/glad.h>

std::map<std::string, RawModel> ResourceManager::RawModels;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture> ResourceManager::Textures;

Shader& ResourceManager::LoadShader(std::string name, Shader shader)
{
	Shaders.emplace(name, shader);
	return Shaders.at(name);
}

Shader& ResourceManager::GetShader(std::string name)
{
	return Shaders.at(name);
}

Texture& ResourceManager::LoadTexture(std::string name, Texture texture)
{
	Textures.emplace(name, texture);
	return Textures.at(name);
}

Texture& ResourceManager::GetTexture(std::string name)
{
	return Textures.at(name);
}

RawModel& ResourceManager::LoadRawModel(std::string name, RawModel model)
{
	RawModels.emplace(name, model);
	return RawModels.at(name);
}

RawModel& ResourceManager::GetRawModel(std::string name)
{
	return RawModels.at(name);
}

void ResourceManager::Clear()
{
	for (auto& iter : Shaders)
		iter.second.deleteShader();
	for (auto& iter : RawModels)
		iter.second.Delete();
	for (auto& iter : Textures)
		iter.second.Delete();
}
