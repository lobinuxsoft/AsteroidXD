#include "HpPowerUp.h"

void HpPowerUp::screenLimitLogic()
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

void HpPowerUp::movement()
{
	if (active)
	{
		// Movement
		position.x += speed.x * GetFrameTime();
		position.y += speed.y * GetFrameTime();
	}
}

void HpPowerUp::updateTimer()
{
	if (!active)
	{
		if (timer > 0)
		{
			timer -= GetFrameTime();
		}
		else
		{
			this->setSpeedByAngle(GetRandomValue(0, 360));
			setActive(true);
		}
	}
}

HpPowerUp::HpPowerUp(Vector2 position, const char spriteUrl[], const char soundfxUrl[], Vector2 speed, float maxSpeed, float rotation, float radius, bool active) : Entity{ position }
{
	this->sprite = LoadTexture(spriteUrl);
	this->soundfx = LoadSound(soundfxUrl);
	this->speed = speed;
	this->maxSpeed = maxSpeed;
	this->rotation = rotation;
	this->radius = radius;
	this->active = active;
}

HpPowerUp::~HpPowerUp()
{
	UnloadTexture(sprite);
	UnloadSound(soundfx);
}

void HpPowerUp::setActive(bool active)
{
	if (this->active && !active)
	{
		PlaySound(soundfx);
		this->timer = maxTimer;
	}

	this->active = active;
}

bool HpPowerUp::getActive()
{
	return active;
}

float HpPowerUp::getRadius()
{
	return radius;
}

void HpPowerUp::setSpeedByAngle(float angle)
{
	this->speed = Vector2{ (float)cos(angle * DEG2RAD) * maxSpeed, (float)sin(angle * DEG2RAD) * maxSpeed };
}

void HpPowerUp::update()
{
	screenLimitLogic();
	movement();
	updateTimer();
}

void HpPowerUp::draw()
{
	if (active)
	{
		DrawTexturePro(
			sprite,
			Rectangle{ 0,0,(float)sprite.width,(float)sprite.height },
			Rectangle{ position.x, position.y, radius * 3, radius * 3 },
			Vector2{ (radius * 3) / 2, (radius * 3) / 2 },
			rotation,
			GREEN);
#if _DEBUG
		DrawCircleV(position, radius, Fade(GREEN, 0.5f));
#endif // _DEBUG
	}
}
