#include "renderer.h"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader_s.h"


void Renderer::render(Entity& entity, Shader& shader)
{
	glBindVertexArray(entity.model->VAO_ID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	shader.activate();

	float aspectRatio = display->displayWidth / display->displayHeight;

	// In a 2d game, we use ortho projection
	// This directly translates display pixel size to NDC
	/*glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), aspectRatio, camera.NEAR_PLANE, camera.FAR_PLANE);*/
	glm::mat4 projection = glm::ortho(0.0f, display->displayWidth, display->displayHeight, 0.0f, -1.0f, 1.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	// Apply entity positions and transformations
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);

	glm::vec3 eulerAngles(glm::radians(entity.rotation.x), glm::radians(entity.rotation.y), glm::radians(entity.rotation.z));
	glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), entity.scale);

	glm::mat4 transform = translate * rotation * scale;
	shader.setMat4("transform", glm::value_ptr(transform));

	// In a 2d game, we don't need a view matrix
	//glm::mat4 view;
	//view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
	//shader.setMat4("view", glm::value_ptr(view));

	entity.ProvideRenderData(shader);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, entity.texture->textureID);

	glDrawElements(GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0);

	shader.deactivate();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Renderer::prepare()
{
	// For 2d game, depth test causes z-index clashing
	//glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
