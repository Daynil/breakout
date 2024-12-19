#include "entity.h"

#include <glm/glm.hpp>

#include <optional>



Entity::Entity(Model* pModel, glm::vec3 pPosition, glm::vec3 pRotation, glm::vec3 pScale, std::optional<glm::vec3> p_rgb_color)
{
	model = pModel;
	position = pPosition;
	rotation = pRotation;
	scale = pScale;

	if (p_rgb_color.has_value())
	{
		rgb_color = p_rgb_color.value();
	}
}
