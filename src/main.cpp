#include <iostream>
#include <cmath>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our stuff
#include "shader_s.h"
#include "display.h"
#include "entity.h"
#include "texture.h"
#include "renderer.h"
#include "controls.h"
#include "model.h"
#include "game.h"


#define USE_GPU_ENGINE 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}


int main(void)
{

	if (!glfwInit())
		return -1;


	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //you might want to do this when testing the game for shipping

    Display display(800.0f, 600.0f, "OpenGL Experiments");
	if (!display.window)
	{
        std::cout << "***ERROR initializing glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}

    Camera camera;
    Controls controls(display.window, &camera);
    Renderer renderer;

    Game game;
    game.Init();
    game.LoadLevel();

    float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(display.window))
	{
        float currentFrame = glfwGetTime();
        // Delta time = seconds per frame (s/f)
        float deltaTime = currentFrame - lastFrame;
        float fps = 1 / deltaTime;
        lastFrame = currentFrame;

        controls.processInput(display.window, deltaTime);
        renderer.prepare();

        //renderer.render(cube, shader, camera, display);
        int idx = 0;
        //for each(Entity cube in cubes)
        for (auto& cube : game.Bricks)
        {
            cube.rotationZ = (float)glfwGetTime() * 20 * idx;
            renderer.render(cube, game.Shaders.at("entity"), camera, display);
            idx += 1;
        }

        //std::cout << gameState.fps << " " << gameState.deltaTime << std::endl;

		glfwSwapBuffers(display.window);
		glfwPollEvents();
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	//glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
