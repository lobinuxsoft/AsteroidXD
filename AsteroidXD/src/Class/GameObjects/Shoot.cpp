#include "Shoot.h"

Shoot::Shoot(Vector2 position, Vector2 speed, float radius, float rotation, int lifeSpawn, bool active, Color color) : 
	Entity { position },
	speed(speed),
	radius(radius),
	rotation(rotation),
	lifeSpawn(lifeSpawn),
	active(active),
	color(color) { }

bool Shoot::getActive()
{
	return active;
}

float Shoot::getRadius()
{
    return radius;
}

float Shoot::getRotation()
{
    return rotation;
}

void Shoot::setActive(bool active)
{
	this->active = active;
}

void Shoot::setSpeed(float rotation, float speed)
{
	this->speed = Vector2 { 1.5f * sin(rotation * DEG2RAD) * speed * GetFrameTime(), 1.5f * cos(rotation * DEG2RAD) * speed * GetFrameTime() };
	this->rotation = rotation;
}

void Shoot::addLifeSpawn()
{
	lifeSpawn++;
}

void Shoot::resetLifeSpawn()
{
	lifeSpawn = 0;
}

void Shoot::update()
{
    move();
    screenLimitsLogic();
    updateLifeSpawn();
}

void Shoot::move()
{
	position.x += speed.x;
	position.y -= speed.y;
}

void Shoot::screenLimitsLogic()
{
    // Collision logic: shoot vs walls
    if (position.x > GetScreenWidth() + radius)
    {
        active = false;
        lifeSpawn = 0;
    }
    else if (position.x < 0 - radius)
    {
        active = false;
        lifeSpawn = 0;
    }
    if (position.y > GetScreenHeight() + radius)
    {
        active = false;
        lifeSpawn = 0;
    }
    else if (position.y < 0 - radius)
    {
        active = false;
        lifeSpawn = 0;
    }
}

void Shoot::updateLifeSpawn()
{
    // Life of shoot
    if (lifeSpawn >= 500)
    {
        position = Vector2{ 0, 0 };
        speed = Vector2{ 0, 0 };
        lifeSpawn = 0;
        active = false;
    }
}

void Shoot::draw()
{
	if (active) DrawCircleV(position, radius, color);
}
