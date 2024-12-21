#include "entity.h"

#include <glm/glm.hpp>

#include <optional>


Entity::Entity(RawModel* pModel, Texture* pTexture, glm::vec3 pPosition, glm::vec3 pRotation, glm::vec3 pScale, std::optional<glm::vec4> p_rgba_color)
{
	model = pModel;
	texture = pTexture;
	position = pPosition;
	rotation = pRotation;
	scale = pScale;

	if (p_rgba_color.has_value())
	{
		rgba_color = p_rgba_color.value();
	}
}
