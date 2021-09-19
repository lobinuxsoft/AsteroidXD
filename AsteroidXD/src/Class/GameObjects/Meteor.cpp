#include "Meteor.h"

Meteor::Meteor(Vector2 position, const char spriteUrl[], const char explodeSfxUrl[], Vector2 speed, float maxSpeed, float rotation, float radius, bool active) : Entity{ position }
{
	this->sprite = LoadTexture(spriteUrl);
	this->explodeSfx = LoadSound(explodeSfxUrl);
	this->speed = speed;
	this->maxSpeed = maxSpeed;
	this->rotation = rotation;
	this->radius = radius;
	this->active = active;
}

Meteor::~Meteor()
{
	UnloadTexture(sprite);
	UnloadSound(explodeSfx);
}

void Meteor::movement()
{
	if (active)
	{
		// Movement
		position.x += speed.x * GetFrameTime();
		position.y += speed.y * GetFrameTime();
	}
}

void Meteor::screenLimitsLogic()
{
	if (active)
	{
		// Collision logic: meteor vs wall
		if (position.x > GetScreenWidth() + radius) position.x = -radius;
		else if (position.x < 0 - radius) position.x = GetScreenWidth() + radius;
		if (position.y > GetScreenHeight() + radius) position.y = -radius;
		else if (position.y < 0 - radius) position.y = GetScreenHeight() + radius;
	}
}

float Meteor::getRadius()
{
	return radius;
}

bool Meteor::getActive()
{
	return active;
}

void Meteor::setActive(bool active)
{
	this->active = active;
}

void Meteor::explode()
{
	setActive(false);
	SetSoundPitch(explodeSfx, ((float)GetRandomValue(0, 45) / 100) + 1);
	PlaySound(explodeSfx);
}

void Meteor::setSpeed(Vector2 speed)
{
	this->speed = speed;
}

void Meteor::setSpeedByAngle(float angle, bool inverseDir)
{
	this->speed = Vector2{ (float)cos(angle * DEG2RAD) * maxSpeed * ((inverseDir) ? -1.0f : 1.0f), (float)sin(angle * DEG2RAD) * maxSpeed * ((inverseDir) ? -1.0f : 1.0f) };
}

void Meteor::update()
{
	movement();
	screenLimitsLogic();
}

void Meteor::draw()
{
	if (active)
	{
		DrawTexturePro(
			sprite,
			Rectangle{ 0,0,(float)sprite.width,(float)sprite.height },
			Rectangle{ position.x, position.y, radius * 3, radius * 3 },
			Vector2{ (radius * 3) / 2, (radius * 3) / 2 },
			rotation,
			WHITE);
#if _DEBUG
		DrawCircleV(position, radius, Fade(RED, 0.5f));
#endif // _DEBUG
	}
}
