#include "Entity.h"

Entity::Entity(Vector2 position) : position(position) { }

void Entity::setPosition(Vector2 position)
{
	this->position = position;
}

Vector2 Entity::getPosition()
{
	return position;
}
