#include "game.h"

#include <vector>

#include <glm/glm.hpp>

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

	Models.emplace("ball", Model(RESOURCES_PATH "awesomeface.png", true, vertices, textureCoords, indices));
    Models.emplace("brick", Model(RESOURCES_PATH "container.jpg", false, vertices, textureCoords, indices));

    Shaders.emplace("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));
}

void Game::LoadLevel()
{
    std::vector<glm::vec3> brick_positions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    for (const auto& brick_position : brick_positions)
    {
        Bricks.push_back(Entity(&Models.at("brick"), brick_position, 0, 0, 0, 1));
    }
}
