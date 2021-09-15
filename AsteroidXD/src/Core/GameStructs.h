#pragma once
#include "Vector2Utils.h"

#pragma region STRUCTS

struct Player
{
private:
    float timer;

public:
    Texture2D sprite;
    Vector2 position;
    Vector2 speed;
    Color color = WHITE;
    int shield;
    float acceleration;
    float rotation;
    float radius;

    bool DamageShip(Vector2 pushDir = { 0,0 })
    {
        timer = 0.5f;
        color = RED;

        position = Vector2Add(position, Vector2Scale(Vector2Normalize(pushDir), radius));
        shield--;

        return shield <= 0;
    }

    void LookAt(Vector2 pointToLook)
    {
        // Player logic: rotation
        if (Vector2Length(Vector2Subtract(pointToLook, position)) > 40.0f)
        {
            rotation = Vector2Angle(position, pointToLook) + 90;
        }
    }

    void MoveForward(bool moving)
    {
        // Player logic: acceleration
        if (moving)
        {
            // Player logic: speed
            speed.x = sin(rotation * DEG2RAD) * PLAYER_SPEED;
            speed.y = cos(rotation * DEG2RAD) * PLAYER_SPEED;

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

    void ScreenLimitsLogic(const int width, const int height)
    {
        // Collision logic: player vs walls
        if (position.x > width + radius) position.x = -radius;
        else if (position.x < -radius) position.x = width + radius;

        if (position.y > (height + radius)) position.y = -radius;
        else if (position.y < -radius) position.y = height + radius;
    }

    void Draw()
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
};

struct Shoot
{
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;

    void Draw()
    {
        if (active) DrawCircleV(position, radius, color);
    }
};

struct Meteor
{
    Texture2D sprite;
    Vector2 position;
    Vector2 speed;
    float rotation;
    float radius;
    bool active;

    void Movement()
    {
        if (active)
        {
            // Movement
            position.x += speed.x * GetFrameTime();
            position.y += speed.y * GetFrameTime();
        }
    }

    void ScreenLimitsLogic(const int width, const int height)
    {
        if (active)
        {
            // Collision logic: meteor vs wall
            if (position.x > width + radius) position.x = -radius;
            else if (position.x < 0 - radius) position.x = width + radius;
            if (position.y > height + radius) position.y = -radius;
            else if (position.y < 0 - radius) position.y = height + radius;
        }
    }

    void Draw()
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
};

struct Button
{
    std::string text = "";
    int fontSize = 10;
    Color textColor = WHITE;
    Color normalColor = BLUE;
    Color pressColor = DARKBLUE;
    Vector2 position{ 0,0 };
    float hMargin = 5;
    float vMargin = 5;
    Rectangle rect{ 0,0,0,0 };
    bool hover = false;
    bool press = false;
    bool click = false;

    void MouseCollision()
    {
        hover = CheckCollisionPointRec(GetMousePosition(), rect);
        press = hover && IsMouseButtonDown(0);
        click = hover && IsMouseButtonReleased(0);
    }

    void Draw()
    {
        rect = Rectangle
        {
            position.x,
            position.y,
            hMargin * 2 + MeasureText(text.c_str(), fontSize),
            vMargin * 2 + fontSize
        };

        DrawRectangleRounded(rect, 0.5f, 12, press ? pressColor : normalColor);
        DrawText(text.c_str(), position.x + hMargin, position.y + vMargin, fontSize, textColor);
    }
};

struct ProgressBar
{
    Vector2 position{ 0, 0 };
    float width = 300;
    float height = 10;
    Color front = BLUE;
    Color back = DARKBLUE;

    void Draw(float value = 1.0f)
    {
        // Shield Bar
        DrawRectangleRoundedLines(Rectangle{ position.x,position.y, width, height }, 1, 12, 5, back);
        DrawRectangleRounded(Rectangle{ position.x, position.y, value * width, height }, 1, 12, front);
    }
};

#pragma endregion