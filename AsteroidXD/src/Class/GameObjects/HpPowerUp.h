#pragma once
#include "Class/Entity.h"

class HpPowerUp : public Entity
{
private:
	const float maxTimer = 5;
	Texture2D sprite;
	Sound soundfx;
	Vector2 speed;
	float maxSpeed;
	float rotation;
	float radius;
	bool active = false;
	float timer = maxTimer;

private:
	void screenLimitLogic();
	void movement();
	void updateTimer();

public:
	HpPowerUp(Vector2 position, const char spriteUrl[], const char soundfxUrl[], Vector2 speed, float maxSpeed, float rotation, float radius, bool active);
	~HpPowerUp();
	void setActive(bool active);
	bool getActive();
	float getRadius();
	void setSpeedByAngle(float angle);
	void update();
	void draw() override;
};

