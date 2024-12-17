#include "controls.h"

#include <string>
#include <iostream>
#include <cmath>

GLFWwindow* Controls::window = nullptr;

Controls::Controls(GLFWwindow* pWindow, Camera* pCamera)
{
    Controls::window = pWindow;
    camera = pCamera;

    glfwSetWindowUserPointer(window, this);

    // Disable cursor for best FPS mode, removed for testing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetJoystickCallback(joystick_callback);

    //glfwSetKeyCallback(window, key_callback);

    // Look for controllers that were already connected before the game started
    for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++)
    {
        if (glfwJoystickPresent(i))
            joystick_callback(i, GLFW_CONNECTED);
    }
}


void Controls::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Retrieve the controls instance associated with this window
    Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

    if (controls->firstMouse)
    {
        controls->lastX = xpos;
        controls->lastY = ypos;
        controls->firstMouse = false;
    }

    float xoffset = xpos - controls->lastX;
    float yoffset = controls->lastY - ypos;
    controls->lastX = xpos;
    controls->lastY = ypos;

    controls->yaw += (xoffset * controls->mouse_sensitivity);
    controls->pitch += (yoffset * controls->mouse_sensitivity);

    if (controls->pitch > 89.0f)
        controls->pitch = 89.0f;
    if (controls->pitch < -89.0f)
        controls->pitch = -89.0f;

    controls->camera->adjustFront(controls->yaw, controls->pitch, controls->roll);
}

void Controls::scroll_callback(GLFWwindow* window, double xoffest, double yoffset)
{
    // Retrieve the controls instance associated with this window
    Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

    controls->camera->FOV -= (float)yoffset;
    if (controls->camera->FOV < 1.0f)
        controls->camera->FOV = 1.0f;
    if (controls->camera->FOV > 45.0f)
        controls->camera->FOV = 45.0f;
}

void Controls::joystick_callback(int jid, int event)
{
    // Retrieve the controls instance associated with this window
    Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(Controls::window));

    if (event == GLFW_CONNECTED)
    {
        if (glfwJoystickIsGamepad(jid))
        {
            std::cout << "Gamepad connected: " + (std::string)glfwGetGamepadName(jid) + " (id: " << jid << ")" << std::endl;
            GLFWgamepadstate state;
            if (glfwGetGamepadState(jid, &state))
            {
                controls->joystick_id = jid;
            }
            else
            {
                std::cout << "Gamepad has no valid mapping: " + (std::string)glfwGetGamepadName(jid) << std::endl;
            }
        }

    }
    else if (event == GLFW_DISCONNECTED)
    {
        //std::cout << "Gamepad disconnected: " + (std::string)glfwGetGamepadName(jid) << std::endl;
        std::cout << "Gamepad id " << jid << " disconnected!" << std::endl;
        controls->joystick_id = -1;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Controls::processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->cameraPos += speed * deltaTime * camera->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->cameraPos -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->cameraPos -= speed * deltaTime * camera->cameraFront;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->cameraPos += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * speed * deltaTime;

    if (joystick_id >= 0)
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(joystick_id, &state))
        {
            float leftStickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            float leftStickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            // Avoid dead zone
            if (std::abs(leftStickY) > 0.1)
                // Invert y (defaults to down is up)
                camera->cameraPos += -leftStickY * speed * deltaTime * camera->cameraFront;
            if (std::abs(leftStickX) > 0.1)
                camera->cameraPos += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * leftStickX * speed * deltaTime;

            float rightStickX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
            float rightStickY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

            // Avoid dead zone
            if (std::abs(rightStickX) > 0.1)
                yaw += (rightStickX * joystick_sensitivity);
            if (std::abs(rightStickY) > 0.1)
                // By default right stick uses airplane style controls (up is down) - invert
                pitch += (-rightStickY * joystick_sensitivity);

            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            camera->cameraFront = glm::normalize(direction);

            for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; ++i) {
                if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
                {
                    std::cout << "A Button pressed!" << std::endl;
                }
                if (state.buttons[i] == GLFW_PRESS) {
                    std::cout << "Button " << i << " pressed!" << std::endl;
                }
            }
        }
    }
}

//static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//
//    if ((key == GLFW_KEY_UP || key == GLFW_KEY_W) && action == GLFW_PRESS)
//        camera->cameraPos += speed * deltaTime * camera->cameraFront;
//    if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_PRESS)
//        camera->cameraPos -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * speed * deltaTime;
//    if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && action == GLFW_PRESS)
//        camera->cameraPos -= speed * deltaTime * camera->cameraFront;
//    if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_PRESS)
//        camera->cameraPos += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * speed * deltaTime;
//}
