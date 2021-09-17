#include "Ship.h"

void Ship::lookAtMousePoint()
{
	// Player logic: rotation
	if (Vector2Length(Vector2Subtract(GetMousePosition(), position)) > 40.0f)
	{
		rotation = Vector2Angle(position, GetMousePosition()) + 90;
        dir = Vector2Normalize(Vector2Subtract(GetMousePosition(), position));
	}
}

void Ship::moveForward()
{
    // Player logic: acceleration
    if (IsMouseButtonDown(1))
    {
        // Player logic: speed
        speed.x = dir.x;
        speed.y = -dir.y;

        velocity = Vector2Add(velocity, { (speed.x * acceleration) * GetFrameTime(), (speed.y * acceleration) * GetFrameTime() });

        if (acceleration < 1) acceleration += maxAcceleration * GetFrameTime();
    }
    else
    {
        acceleration = 0;
    }

    velocity = { Clamp(velocity.x, -maxVelocity, maxVelocity), Clamp(velocity.y, -maxVelocity, maxVelocity) };

    // Player logic: movement
    position.x += velocity.x;
    position.y -= velocity.y;
}

Ship::Ship(Vector2 position, const char spriteUrl[]) : Entity{ position }, sprite(LoadTexture(spriteUrl)) { }

Ship::~Ship()
{
    UnloadTexture(sprite);
}

float Ship::getRotation()
{
    return rotation;
}

float Ship::getRadius()
{
    return radius;
}

int Ship::getShield()
{
    return shield;
}

int Ship::getMaxShield()
{
    return maxShield;
}

void Ship::resetState()
{
    velocity = { 0,0 };
    speed = { 0,0 };
    acceleration = 0;
    shield = maxShield;
}

float Ship::getMaxSpeed()
{
    return maxAcceleration;
}

bool Ship::damageShip(Vector2 hitPos)
{
    Vector2 pushDir = Vector2Subtract(position, hitPos);
	timer = 0.5f;
	color = RED;

    velocity.x += Vector2Normalize(pushDir).x;
    velocity.y -= Vector2Normalize(pushDir).y;
	shield--;

	return shield <= 0;
}

void Ship::screenLimitsLogic()
{
    // Collision logic: player vs walls
    if (position.x > GetScreenWidth() + radius) position.x = -radius;
    else if (position.x < -radius) position.x = GetScreenWidth() + radius;

    if (position.y > (GetScreenHeight() + radius)) position.y = -radius;
    else if (position.y < -radius) position.y = GetScreenHeight() + radius;
}

void Ship::update()
{
    lookAtMousePoint();
    moveForward();
    screenLimitsLogic();
}

void Ship::draw()
{
    if (timer > 0)
    {
        timer -= GetFrameTime();
    }
    else
    {
        color = WHITE;
    }

    // Draw spaceship
    DrawTexturePro(
        sprite,
        Rectangle{ 0,0,(float)sprite.width,(float)sprite.height },
        Rectangle{ position.x, position.y, (float)sprite.width * 0.3f,(float)sprite.height * 0.3f },
        Vector2{ ((float)sprite.width * 0.3f) / 2, ((float)sprite.height * 0.3f) / 2 },
        rotation,
        color);

    // Draw collision
#if _DEBUG
    DrawCircle(position.x, position.y, 18.0f, Fade(GREEN, 0.5f));
    DrawText(TextFormat("Dir (%02.02f,%02.02f)", dir.x, dir.y), 10, GetScreenHeight() * 0.4f, 20, WHITE);
    DrawText(TextFormat("Velocity (%02.02f,%02.02f)", velocity.x, velocity.y), 10, GetScreenHeight() * 0.5f, 20, WHITE);
#endif // _DEBUG
}
