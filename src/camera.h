#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float FOV = 45.0f;
    float NEAR_PLANE = 0.1f;
    float FAR_PLANE = 100.0f;

	Camera();

    void adjustFront(float yaw, float pitch, float roll);
};