#pragma once
#include <cmath>
#include "raylib.h"

#pragma region VECTOR2 HELPERS

float Clamp(float value, float min, float max);

/// <summary>
/// Angulo que se genera con 2 vetores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Vector2Angle(Vector2 v1, Vector2 v2);

/// <summary>
/// Calcula la distancia entre 2 vectores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
float Vector2Distance(Vector2 v1, Vector2 v2);

/// <summary>
/// El largo del vector o la magnitud
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
float Vector2Length(Vector2 v);

/// <summary>
/// Escalar el vector
/// </summary>
/// <param name="v"></param>
/// <param name="scale"></param>
/// <returns></returns>
Vector2 Vector2Scale(Vector2 v, float scale);

Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);

/// <summary>
/// Normalizar el vector
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
Vector2 Vector2Normalize(Vector2 v);

Vector2 Vector2Add(Vector2 v1, Vector2 v2);

/// <summary>
/// Resta entre 2 vectores
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);

#pragma endregion