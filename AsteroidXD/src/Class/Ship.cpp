#include "Ship.h"

void Ship::lookAtMousePoint()
{
	// Player logic: rotation
	if (Vector2Length(Vector2Subtract(GetMousePosition(), position)) > 40.0f)
	{
		rotation = Vector2Angle(position, GetMousePosition()) + 90;
	}
}

void Ship::moveForward()
{
    // Player logic: acceleration
    if (IsMouseButtonDown(1))
    {
        // Player logic: speed
        speed.x = sin(rotation * DEG2RAD) * maxSpeed;
        speed.y = cos(rotation * DEG2RAD) * maxSpeed;

        if (acceleration < 1) acceleration += 0.04f;
    }
    else
    {
        if (acceleration > 0) acceleration -= 0.005f;
        else if (acceleration < 0) acceleration = 0;
    }

    // Player logic: movement
    position.x += (speed.x * acceleration) * GetFrameTime();
    position.y -= (speed.y * acceleration) * GetFrameTime();
}

Ship::Ship(Vector2 position, const char spriteUrl[], float maxSpeed, int shield, float radius) : Entity{ position }
{
	this->sprite = LoadTexture(spriteUrl);
    this->maxSpeed = maxSpeed;
	this->shield = shield;
	this->radius = radius;
}

Ship::~Ship()
{
    UnloadTexture(sprite);
}

Vector2 Ship::getPosition()
{
    return this->position;
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

void Ship::setShield(int shield)
{
    this->shield = shield;
}

bool Ship::damageShip(Vector2 hitPos)
{
    Vector2 pushDir = Vector2Subtract(position, hitPos);
	timer = 0.5f;
	color = RED;

	position = Vector2Add(position, Vector2Scale(Vector2Normalize(pushDir), radius));
	shield--;

	return shield <= 0;
}

void Ship::screenLimitsLogic(const int width, const int height)
{
    // Collision logic: player vs walls
    if (position.x > width + radius) position.x = -radius;
    else if (position.x < -radius) position.x = width + radius;

    if (position.y > (height + radius)) position.y = -radius;
    else if (position.y < -radius) position.y = height + radius;
}

void Ship::update()
{
    lookAtMousePoint();
    moveForward();
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
#endif // _DEBUG
}
