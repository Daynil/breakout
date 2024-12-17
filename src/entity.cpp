#include "entity.h"

#include <glm/glm.hpp>


Entity::Entity(Model* pModel, glm::vec3 pPosition, float pRotationX, float pRotationY, float pRotationZ, float pScale)
{
    rotation = { pRotationX, pRotationY, pRotationZ };
    model = pModel;
    position = pPosition;
    rotationX = pRotationX;
    rotationY = pRotationY;
    rotationZ = pRotationZ;
    scale = pScale;
}
