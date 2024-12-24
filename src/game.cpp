#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <raudio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "resource_manager.h"

int BRICK_WIDTH = 50;
int BRICK_HEIGHT = 25;

unsigned int textVAO, textVBO;

struct Character {
	// ID handle of the glyph texture
	unsigned int TextureID;
	// Size of glyph
	glm::ivec2   Size;
	// Offset from baseline to left/top of glyph
	glm::ivec2   Bearing;
	// Offset to advance to next glyph
	unsigned int Advance;
};

std::map<char, Character> Characters;

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{
	s.activate();
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);
	s.setMat4("projection", glm::value_ptr(projection));
	s.setInt("text", 0);
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// Update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 0.0f },

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		// bitshift by 6 to get value in pixels (2^6 = 64)
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::Init()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, RESOURCES_PATH "arial.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	/*if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
		return;
	}*/

	// disable byte-alignment restriction
	// Each glyph is a grayscale 8-bit image, so we store each
	// byte of the bitmap buffer as the texture's single color value.
	// Normally, we always store 4 bytes per pixel (rgba).
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
			return;
		}
		// generate texture
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Stash character data for later use
		Character character = {
			textureID,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Load text buffers
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// End load text buffers


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

	ResourceManager::LoadRawModel("quad", RawModel(vertices, textureCoords, indices));

	ResourceManager::LoadTexture("background", Texture(RESOURCES_PATH "background.jpg", false));
	ResourceManager::LoadTexture("paddle", Texture(RESOURCES_PATH "paddle.png", true));
	ResourceManager::LoadTexture("ball", Texture(RESOURCES_PATH "ball.png", true));
	ResourceManager::LoadTexture("wood", Texture(RESOURCES_PATH "container.jpg", false));
	ResourceManager::LoadTexture("brick", Texture(RESOURCES_PATH "brick.png", true));
	ResourceManager::LoadTexture("metal", Texture(RESOURCES_PATH "metal.png", true));
	ResourceManager::LoadTexture("particle", Texture(RESOURCES_PATH "particle.png", true));

	ResourceManager::LoadShader("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));
	ResourceManager::LoadShader("entity_tinted", Shader(RESOURCES_PATH "shaders/entity_tinted.shader"));
	ResourceManager::LoadShader("particle", Shader(RESOURCES_PATH "shaders/particle.shader"));
	ResourceManager::LoadShader("text", Shader(RESOURCES_PATH "shaders/text.shader"));

	InitAudioDevice();
	//ResourceManager::LoadRSound("brick", RESOURCES_PATH "bleep.mp3");
	ResourceManager::LoadRSound("brick", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("brick_solid", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("powerup", RESOURCES_PATH "powerup.wav");
	ResourceManager::LoadRSound("paddle", RESOURCES_PATH "bleep.wav");

	LoadLevel(3);

	glm::vec3 player_size = glm::vec3(100.0f, 20.0f, 0.0f);
	glm::vec3 playerPos = glm::vec3(
		LevelWidth / 2.0f - player_size.x / 2.0f,
		LevelHeight - player_size.y, 0.0f
	);
	player = new Player(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("paddle"),
		playerPos,
		glm::vec3(0),
		player_size
	);

	glm::vec3 initial_ball_velocity = glm::vec3(100.0f, -350.0f, 0.0f);
	float ball_radius = 10.0f;
	ball = new Ball(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("ball"),
		glm::vec3(0),
		glm::vec3(0),
		glm::vec3(ball_radius * 2, ball_radius * 2, 0.0f),
		player
	);
	ball->Reset();

	particle_manager = new ParticleManager(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("particle"),
		&ResourceManager::GetShader("particle"),
		ball
	);
	particle_manager->Init(800);


}

void Game::LoadLevel(int level)
{
	int max_bricks_x = LevelWidth / BRICK_WIDTH;
	int max_bricks_y = LevelHeight / BRICK_HEIGHT * 0.75;

	std::ifstream level_stream(RESOURCES_PATH "level" + std::to_string(level) + ".txt");
	std::string line;
	std::stringstream ss;

	int longest_row = 0;
	int tallest_col = 0;

	RawModel* model = &ResourceManager::GetRawModel("quad");

	int row = 0;
	while (std::getline(level_stream, line))
	{
		int col = 0;
		for (auto& brick_type : line)
		{
			Texture* texture;
			int life = 0;
			glm::vec4 color = glm::vec4(0);
			if (brick_type == '0') {
				col += 1;
				if (col > longest_row)
					longest_row = col;
				continue;
			}
			else if (brick_type == '1') {
				life = 999;
				texture = &ResourceManager::GetTexture("metal");
			}
			else if (brick_type == '2') {
				life = 1;
				texture = &ResourceManager::GetTexture("wood");
			}
			else {
				int brick_numeric = std::stoi(std::string(1, brick_type));
				life = brick_numeric - 1;
				// Regular brick no color for 2
				if (brick_numeric == 3)
					color = glm::vec4(0, 0.5, 0, 0.55);
				if (brick_numeric == 4)
					color = glm::vec4(0, 0, 0.5, 0.55);
				if (brick_numeric == 5)
					color = glm::vec4(0.5, 0, 0, 0.55);

				texture = &ResourceManager::GetTexture("brick");
			}

			Bricks.push_back(
				Brick(
					model, texture, glm::vec3(BRICK_WIDTH * col, BRICK_HEIGHT * row, 0), glm::vec3(0), glm::vec3(BRICK_WIDTH, BRICK_HEIGHT, 0), color,
					life
				)
			);

			col += 1;
			if (col > longest_row)
				longest_row = col;
		}
		row += 1;
		if (row > tallest_col)
			tallest_col = row;
	}

	int bricks_shift_x = (max_bricks_x - longest_row) / 2;
	int bricks_shift_y = (max_bricks_y - tallest_col) / 2;

	for (auto& brick : Bricks)
	{
		brick.position.x += (bricks_shift_x * BRICK_WIDTH);
		brick.position.y += (bricks_shift_y * BRICK_HEIGHT);
	}
}

void Game::ProcessInput(float dt)
{
	if (State != GAME_ACTIVE)
		return;

	float player_movement = 0;
	float should_release = false;

	// Keyboard controls
	if (keyboard_keys[GLFW_KEY_A] || keyboard_keys[GLFW_KEY_LEFT])
		player_movement = -1;
	if (keyboard_keys[GLFW_KEY_D] || keyboard_keys[GLFW_KEY_RIGHT])
		player_movement = 1;
	if (keyboard_keys[GLFW_KEY_SPACE])
		should_release = true;

	// Gamepad controls
	if (left_stick_x != 0)
		player_movement = left_stick_x;

	if (gamepad_keys[GLFW_GAMEPAD_BUTTON_X])
		should_release = true;

	player->Move(dt, LevelWidth, player_movement);
	ball->Move(dt, LevelWidth, LevelHeight, should_release, player_movement);
}

void Game::Update(float dt)
{
	CheckCollisions();
	particle_manager->Update(dt);
}

void Game::Render()
{
	renderer->prepare();

	renderer->render(Entity(&ResourceManager::GetRawModel("quad"), &ResourceManager::GetTexture("background"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(LevelWidth, LevelHeight, 0)), ResourceManager::GetShader("entity"));
	for (auto& brick : Bricks)
	{
		if (brick.life > 0)
			renderer->render(brick, ResourceManager::GetShader("entity_tinted"));
	}
	renderer->render(*player, ResourceManager::GetShader("entity"));
	renderer->render(*ball, ResourceManager::GetShader("entity"));

	particle_manager->Render(*renderer);
	RenderText(ResourceManager::GetShader("text"), "Lives: 3", 5.0f, 5.0f, 1.0f, glm::vec3(1.0f));
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::CheckCollisions()
{
	for (Brick& brick : Bricks) {
		if (brick.life > 0) {
			Collision collision(CheckCollision(*ball, brick));
			if (collision.occured) {
				brick.life = 0;
				PlaySound(ResourceManager::GetSound("brick"));

				if (collision.dir == LEFT || collision.dir == RIGHT) {
					ball->velocity.x = -ball->velocity.x;
					float penetration = (ball->scale.x / 2.0f) - std::abs(collision.diff.x);
					if (collision.dir == LEFT)
						ball->position.x += penetration;
					else
						ball->position.x -= penetration;
				}
				else {
					ball->velocity.y = -ball->velocity.y;
					float penetration = (ball->scale.x / 2.0f) - std::abs(collision.diff.y);
					if (collision.dir == UP)
						ball->position.y -= penetration;
					else
						ball->position.y += penetration;
				}
			}
		}
	}

	if (!ball->stuck) {
		Collision collision(CheckCollision(*ball, *player));
		if (collision.occured) {
			PlaySound(ResourceManager::GetSound("paddle"));
			float center_board = player->position.x + player->scale.x / 2.0f;
			float dist = (ball->position.x + (ball->scale.x / 2.0f) - center_board);
			float percentage = dist / (player->scale.x / 2.0f);
			float strength = 2.0f;
			glm::vec2 old_velocity = ball->velocity;
			ball->velocity.x = ball->initial_velocity_x * percentage * strength;
			ball->velocity.y = -1.0f * std::abs(ball->velocity.y);
			ball->velocity = glm::normalize(ball->velocity) * glm::length(old_velocity);
		}
	}
}

Collision Game::CheckCollision(Ball& ball, Entity& two)
{
	// Simple collision check if both are square colliders
	//bool x_overlap = (one.position.x + one.scale.x) >= two.position.x && one.position.x <= (two.position.x + two.scale.x);
	//bool y_overlap = (one.position.y + one.scale.y) >= two.position.y && one.position.y <= (two.position.y + two.scale.y);
	//return x_overlap && y_overlap;

	float ball_radius = ball.scale.x / 2.0f;

	glm::vec2 center(ball.position + ball_radius);
	glm::vec2 aabb_half_extents(two.scale.x / 2.0f, two.scale.y / 2.0f);
	glm::vec2 aabb_center(
		two.position.x + aabb_half_extents.x,
		two.position.y + aabb_half_extents.y
	);

	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	bool collision = glm::length(difference) < ball_radius;
	Direction dir = VectorDirection(difference);

	return Collision{ collision, dir, difference };
}

Game::~Game()
{
	delete player;
}