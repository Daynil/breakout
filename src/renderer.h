#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "entity.h"
#include "camera.h"
#include "display.h"
#include "shader_s.h"

class Renderer
{
public:
	Renderer();

	void render(Entity& entity, Shader& shader, Camera& camera, Display& display);
	void prepare();
};