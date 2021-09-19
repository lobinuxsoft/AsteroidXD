#pragma once
#include "Class/Entity.h"
class Shoot : public Entity
{
private:
	Vector2 speed;
	float radius;
	float rotation;
	int lifeSpawn;
	bool active;
	Color color;

private:
	void move();
	void screenLimitsLogic();
	void updateLifeSpawn();

public:
	Shoot(Vector2 position, Vector2 speed, float radius, float rotation, int lifeSpawn, bool active, Color color);
	bool getActive();
	float getRadius();
	float getRotation();
	void setActive(bool active);
	void setSpeed(float rotation, float speed);
	void addLifeSpawn();
	void resetLifeSpawn();
	void update();
	void draw() override;
};

