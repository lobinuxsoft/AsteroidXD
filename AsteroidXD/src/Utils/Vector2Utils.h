#pragma once
#include <cmath>
#include "raylib.h"

#pragma region VECTOR2 HELPERS

/// <summary>
/// Angulo que se genera con 2 vetores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Vector2Angle(Vector2 v1, Vector2 v2)
{
    float result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / PI);
    if (result < 0) result += 360.0f;
    return result;
}

/// <summary>
/// Calcula la distancia entre 2 vectores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Vector2Distance(Vector2 v1, Vector2 v2)
{
    return sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

/// <summary>
/// El largo del vector o la magnitud
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
float Vector2Length(Vector2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

/// <summary>
/// Escalar el vector
/// </summary>
/// <param name="v"></param>
/// <param name="scale"></param>
/// <returns></returns>
Vector2 Vector2Scale(Vector2 v, float scale)
{
    return Vector2{ v.x * scale, v.y * scale };
}

/// <summary>
/// Normalizar el vector
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vector2 Vector2Normalize(Vector2 v)
{
    float length = Vector2Length(v);
    if (length <= 0)
        return v;

    return Vector2Scale(v, 1 / length);;
}

Vector2 Vector2Add(Vector2 v1, Vector2 v2)
{
    return Vector2{ v1.x + v2.x, v1.y + v2.y };
}

/// <summary>
/// Resta entre 2 vectores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
    return Vector2{ v1.x - v2.x, v1.y - v2.y };
}

#pragma endregion