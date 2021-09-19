#pragma once
#include "Class/Entity.h"

class Meteor : public Entity
{
private:
	Texture2D sprite;
	Sound explodeSfx;
	Vector2 speed;
	float maxSpeed;
	float rotation;
	float radius;
	bool active;

private:
	void movement();
	void screenLimitsLogic();

public:
	Meteor(Vector2 position, const char spriteUrl[], const char explodeSfxUrl[], Vector2 speed, float maxSpeed, float rotation, float radius, bool active);
	~Meteor();
	float getRadius();
	bool getActive();
	void setActive(bool active);
	void explode();
	void setSpeed(Vector2 speed);
	void setSpeedByAngle(float angle, bool inverseDir = false);
	void update();
	void draw() override;
};

