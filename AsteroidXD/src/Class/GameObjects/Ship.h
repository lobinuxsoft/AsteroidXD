#pragma once
#include "../Entity.h"

class Ship : public Entity
{
private:
    const int maxShield = 100;
    const float defaultRadius = 18.0;
    const float maxVelocity = 3;

    Texture2D sprite;
    Color color = WHITE;
    Vector2 velocity{ 0,0 };
    Vector2 dir{ 0,0 };
    Vector2 speed{ 0,0 };
    float maxAcceleration = 150.0f;
    int shield = maxShield;
    float acceleration = 0.0f;
    float rotation = 0.0f;
    float radius = defaultRadius;
    float timer = 0.0f;

private:
    void lookAtMousePoint();
    void moveForward();
    void screenLimitsLogic();

public:
    Ship(Vector2 position, const char spriteUrl[]);
    ~Ship();
    float getRotation();
    float getRadius();
    int getShield();
    int getMaxShield();
    void resetState();
    float getMaxSpeed();
    bool damageShip(Vector2 hitPos = { 0,0 });
    void update();
    void draw() override;
};

