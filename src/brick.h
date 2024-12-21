#pragma once

#include "entity.h"

class Brick
{
public:
	Entity entity;
	int life;

	Brick(Entity entity, int life) : entity(entity), life(life) {};
};