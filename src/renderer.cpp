#include "renderer.h"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader_s.h"

Renderer::Renderer()
{
}

void Renderer::render(Entity& entity, Shader& shader, Camera& camera, Display& display)
{
	glBindVertexArray(entity.model->VAO_ID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	shader.activate();

	float aspectRatio = display.displayWidth / display.displayHeight;
	glm::mat4 perspective = glm::perspective(glm::radians(camera.FOV), aspectRatio, camera.NEAR_PLANE, camera.FAR_PLANE);
	shader.setMat4("projection", glm::value_ptr(perspective));

	// Apply entity positions and transformations
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);

	glm::vec3 eulerAngles(glm::radians(entity.rotationX), glm::radians(entity.rotationY), glm::radians(entity.rotationZ));
	glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(entity.scale, entity.scale, entity.scale));

	glm::mat4 transform = translate * rotation * scale;
	shader.setMat4("transform", glm::value_ptr(transform));

	glm::mat4 view;
	view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
	shader.setMat4("view", glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, entity.model->texture.textureID);

	glDrawElements(GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Renderer::prepare()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
