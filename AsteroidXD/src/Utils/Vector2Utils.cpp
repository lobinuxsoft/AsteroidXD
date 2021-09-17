#include "Vector2Utils.h"

float Clamp(float value, float min, float max)
{
    const float res = value < min ? min : value;
    return res > max ? max : res;
}

float Vector2Angle(Vector2 v1, Vector2 v2)
{
    float result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / PI);
    if (result < 0) result += 360.0f;
    return result;
}

float Vector2Distance(Vector2 v1, Vector2 v2)
{
    return sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

float Vector2Length(Vector2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

Vector2 Vector2Scale(Vector2 v, float scale)
{
    return Vector2{ v.x * scale, v.y * scale };
}

Vector2 Vector2Multiply(Vector2 v1, Vector2 v2)
{
    return Vector2{ v1.x * v2.x, v1.y * v2.y };
}

Vector2 Vector2Normalize(Vector2 v)
{
    float length = Vector2Length(v);
    if (length <= 0)
        return v;

    return Vector2Scale(v, 1 / length);
}

Vector2 Vector2Add(Vector2 v1, Vector2 v2)
{
    return Vector2{ v1.x + v2.x, v1.y + v2.y };
}

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
    return Vector2{ v1.x - v2.x, v1.y - v2.y };
}
