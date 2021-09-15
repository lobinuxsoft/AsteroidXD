#pragma once
#include "Entity.h"

class Ship : public Entity
{
private:
    Texture2D sprite;
    Color color = WHITE;
    Vector2 speed{ 0,0 };
    float maxSpeed = 0.0f;
    int shield = 0;
    float acceleration = 0.0f;
    float rotation = 0.0f;
    float radius = 0.0f;
    float timer = 0.0f;

private:
    void lookAtMousePoint();
    void moveForward();

public:
    Ship(Vector2 position, const char spriteUrl[],float maxSpeed, int shield, float radius);
    ~Ship();
    Vector2 getPosition();
    float getRotation();
    float getRadius();
    int getShield();
    void setShield(int shield);
    bool damageShip(Vector2 hitPos = { 0,0 });
    void screenLimitsLogic(const int width, const int height);
    void update();
    void draw() override;
};

