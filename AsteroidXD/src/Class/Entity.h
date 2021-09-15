#pragma once
#include "../Utils/Vector2Utils.h"

class Entity
{
protected:
	Vector2 position;

public:
	Entity(Vector2 position);
	void setPosition(Vector2 position);
	virtual void draw() = 0;
};

