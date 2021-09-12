#include "GameManager.h"
#include "raylib.h"
#include <math.h>

#pragma region DEFINES

#define PLAYER_BASE_SIZE    18.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   10
#define PLAYER_MAX_SHIELD   100

#define METEORS_SPEED       2
#define MAX_BIG_METEORS     4
#define MAX_MEDIUM_METEORS  8
#define MAX_SMALL_METEORS   16

#pragma endregion

#pragma region STRUCTS

struct Player {
    Texture2D sprite;
    Vector2 position;
    Vector2 speed;
    int shield;
    float acceleration;
    float rotation;
    float radius;

    bool DamageShip()
    {
        shield--;

        return shield <= 0;
    }
};

struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
};

struct Meteor {
    Texture2D sprite;
    Vector2 position;
    Vector2 speed;
    float rotation;
    float radius;
    bool active;
};

#pragma endregion

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

#pragma region GLOBAL VARIABLES

static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;
static bool victory = false;

// NOTE: Defined triangle is isosceles with common angles of 70 degrees.
static float shipRadius = 0;

static Player player;
static Shoot shoot[PLAYER_MAX_SHOOTS];
static Meteor bigMeteor[MAX_BIG_METEORS];
static Meteor mediumMeteor[MAX_MEDIUM_METEORS];
static Meteor smallMeteor[MAX_SMALL_METEORS];

static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;

#pragma endregion

// Initialize game variables
void InitGame()
{
    float posx, posy;
    float velx, vely;
    bool correctRange = false;
    victory = false;
    pause = false;

    shipRadius = PLAYER_BASE_SIZE;

    // Initialization player
    player.sprite = LoadTexture("resources/images/ship_G.png");
    player.position = Vector2{ screenWidth / 2 - shipRadius / 2, screenHeight / 2 - shipRadius / 2 };
    player.speed = Vector2{ 0, 0 };
    player.acceleration = 0;
    player.rotation = 0;
    player.radius = shipRadius;
    player.shield = PLAYER_MAX_SHIELD;

    destroyedMeteorsCount = 0;

    // Initialization shoot
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = Vector2{ 0, 0 };
        shoot[i].speed = Vector2{ 0, 0 };
        shoot[i].radius = 2;
        shoot[i].active = false;
        shoot[i].lifeSpawn = 0;
        shoot[i].color = WHITE;
    }

    for (int i = 0; i < MAX_BIG_METEORS; i++)
    {
        posx = GetRandomValue(0, screenWidth);

        while (!correctRange)
        {
            if (posx > screenWidth / 2 - 150 && posx < screenWidth / 2 + 150) posx = GetRandomValue(0, screenWidth);
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, screenHeight);

        while (!correctRange)
        {
            if (posy > screenHeight / 2 - 150 && posy < screenHeight / 2 + 150)  posy = GetRandomValue(0, screenHeight);
            else correctRange = true;
        }

        bigMeteor[i].position = Vector2{ posx, posy };

        correctRange = false;
        velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
        vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
                vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
            }
            else correctRange = true;
        }

        bigMeteor[i].sprite = LoadTexture("resources/images/meteor_detailedLarge.png");
        bigMeteor[i].speed = Vector2{ velx, vely };
        bigMeteor[i].rotation = GetRandomValue(0, 360);
        bigMeteor[i].radius = 40;
        bigMeteor[i].active = true;
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
    {
        mediumMeteor[i].sprite = LoadTexture("resources/images/meteor_detailedLarge.png");
        mediumMeteor[i].position = Vector2{ -100, -100 };
        mediumMeteor[i].speed = Vector2{ 0,0 };
        mediumMeteor[i].rotation = GetRandomValue(0, 360);
        mediumMeteor[i].radius = 20;
        mediumMeteor[i].active = false;
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++)
    {
        smallMeteor[i].sprite = LoadTexture("resources/images/meteor_detailedLarge.png");
        smallMeteor[i].position = Vector2{ -100, -100 };
        smallMeteor[i].speed = Vector2{ 0,0 };
        smallMeteor[i].rotation = GetRandomValue(0, 360);
        smallMeteor[i].radius = 10;
        smallMeteor[i].active = false;
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;

    HideCursor();
}

// Update game (one frame)
void UpdateGame()
{
    if (!gameOver)
    {

        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Player logic: rotation
            if (Vector2Length(Vector2Subtract(GetMousePosition(), player.position)) > 40.0f)
            {
                player.rotation = Vector2Angle(player.position, GetMousePosition()) + 90;
            }

            // Player logic: speed
            player.speed.x = sin(player.rotation * DEG2RAD) * PLAYER_SPEED;
            player.speed.y = cos(player.rotation * DEG2RAD) * PLAYER_SPEED;

            // Player logic: acceleration
            if (IsMouseButtonDown(1))
            {
                if (player.acceleration < 1) player.acceleration += 0.04f;
            }
            else
            {
                if (player.acceleration > 0) player.acceleration -= 0.02f;
                else if (player.acceleration < 0) player.acceleration = 0;
            }

            // Player logic: movement
            player.position.x += (player.speed.x * player.acceleration);
            player.position.y -= (player.speed.y * player.acceleration);

            // Collision logic: player vs walls
            if (player.position.x > screenWidth + shipRadius) player.position.x = -(shipRadius);
            else if (player.position.x < -(shipRadius)) player.position.x = screenWidth + shipRadius;
            if (player.position.y > (screenHeight + shipRadius)) player.position.y = -(shipRadius);
            else if (player.position.y < -(shipRadius)) player.position.y = screenHeight + shipRadius;

            // Player shoot logic
            if (IsMouseButtonPressed(0))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot[i].active)
                    {
                        shoot[i].position = Vector2{ player.position.x + (float)sin(player.rotation * DEG2RAD) * (shipRadius), player.position.y - (float)cos(player.rotation * DEG2RAD) * (shipRadius) };
                        shoot[i].active = true;
                        shoot[i].speed.x = 1.5 * sin(player.rotation * DEG2RAD) * PLAYER_SPEED;
                        shoot[i].speed.y = 1.5 * cos(player.rotation * DEG2RAD) * PLAYER_SPEED;
                        shoot[i].rotation = player.rotation;
                        break;
                    }
                }
            }

            // Shoot life timer
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) shoot[i].lifeSpawn++;
            }

            // Shot logic
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    // Movement
                    shoot[i].position.x += shoot[i].speed.x;
                    shoot[i].position.y -= shoot[i].speed.y;

                    // Collision logic: shoot vs walls
                    if (shoot[i].position.x > screenWidth + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.x < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    if (shoot[i].position.y > screenHeight + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.y < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }

                    // Life of shoot
                    if (shoot[i].lifeSpawn >= 60)
                    {
                        shoot[i].position = Vector2{ 0, 0 };
                        shoot[i].speed = Vector2{ 0, 0 };
                        shoot[i].lifeSpawn = 0;
                        shoot[i].active = false;
                    }
                }
            }

            for (int a = 0; a < MAX_BIG_METEORS; a++)
            {
                if (CheckCollisionCircles(player.position, player.radius, bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active)
                {

                    gameOver = player.DamageShip();
                }
            }

            for (int a = 0; a < MAX_MEDIUM_METEORS; a++)
            {
                if (CheckCollisionCircles(player.position, player.radius, mediumMeteor[a].position, mediumMeteor[a].radius) && mediumMeteor[a].active)
                {
                    gameOver = player.DamageShip();
                }
            }

            for (int a = 0; a < MAX_SMALL_METEORS; a++)
            {
                if (CheckCollisionCircles(player.position, player.radius, smallMeteor[a].position, smallMeteor[a].radius) && smallMeteor[a].active)
                {
                    gameOver = player.DamageShip();
                }
            }

            // Meteors logic: big meteors
            for (int i = 0; i < MAX_BIG_METEORS; i++)
            {
                if (bigMeteor[i].active)
                {
                    // Movement
                    bigMeteor[i].position.x += bigMeteor[i].speed.x;
                    bigMeteor[i].position.y += bigMeteor[i].speed.y;

                    // Collision logic: meteor vs wall
                    if (bigMeteor[i].position.x > screenWidth + bigMeteor[i].radius) bigMeteor[i].position.x = -(bigMeteor[i].radius);
                    else if (bigMeteor[i].position.x < 0 - bigMeteor[i].radius) bigMeteor[i].position.x = screenWidth + bigMeteor[i].radius;
                    if (bigMeteor[i].position.y > screenHeight + bigMeteor[i].radius) bigMeteor[i].position.y = -(bigMeteor[i].radius);
                    else if (bigMeteor[i].position.y < 0 - bigMeteor[i].radius) bigMeteor[i].position.y = screenHeight + bigMeteor[i].radius;
                }
            }

            // Meteors logic: medium meteors
            for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
            {
                if (mediumMeteor[i].active)
                {
                    // Movement
                    mediumMeteor[i].position.x += mediumMeteor[i].speed.x;
                    mediumMeteor[i].position.y += mediumMeteor[i].speed.y;

                    // Collision logic: meteor vs wall
                    if (mediumMeteor[i].position.x > screenWidth + mediumMeteor[i].radius) mediumMeteor[i].position.x = -(mediumMeteor[i].radius);
                    else if (mediumMeteor[i].position.x < 0 - mediumMeteor[i].radius) mediumMeteor[i].position.x = screenWidth + mediumMeteor[i].radius;
                    if (mediumMeteor[i].position.y > screenHeight + mediumMeteor[i].radius) mediumMeteor[i].position.y = -(mediumMeteor[i].radius);
                    else if (mediumMeteor[i].position.y < 0 - mediumMeteor[i].radius) mediumMeteor[i].position.y = screenHeight + mediumMeteor[i].radius;
                }
            }

            // Meteors logic: small meteors
            for (int i = 0; i < MAX_SMALL_METEORS; i++)
            {
                if (smallMeteor[i].active)
                {
                    // Movement
                    smallMeteor[i].position.x += smallMeteor[i].speed.x;
                    smallMeteor[i].position.y += smallMeteor[i].speed.y;

                    // Collision logic: meteor vs wall
                    if (smallMeteor[i].position.x > screenWidth + smallMeteor[i].radius) smallMeteor[i].position.x = -(smallMeteor[i].radius);
                    else if (smallMeteor[i].position.x < 0 - smallMeteor[i].radius) smallMeteor[i].position.x = screenWidth + smallMeteor[i].radius;
                    if (smallMeteor[i].position.y > screenHeight + smallMeteor[i].radius) smallMeteor[i].position.y = -(smallMeteor[i].radius);
                    else if (smallMeteor[i].position.y < 0 - smallMeteor[i].radius) smallMeteor[i].position.y = screenHeight + smallMeteor[i].radius;
                }
            }

            // Collision logic: player-shoots vs meteors
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if ((shoot[i].active))
                {
                    for (int a = 0; a < MAX_BIG_METEORS; a++)
                    {
                        if (bigMeteor[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, bigMeteor[a].position, bigMeteor[a].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            bigMeteor[a].active = false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j++)
                            {
                                if (midMeteorsCount % 2 == 0)
                                {
                                    mediumMeteor[midMeteorsCount].position = Vector2{ bigMeteor[a].position.x, bigMeteor[a].position.y };
                                    mediumMeteor[midMeteorsCount].speed = Vector2{ (float)cos(shoot[i].rotation * DEG2RAD) * METEORS_SPEED * -1, (float)sin(shoot[i].rotation * DEG2RAD) * METEORS_SPEED * -1 };
                                }
                                else
                                {
                                    mediumMeteor[midMeteorsCount].position = Vector2{ bigMeteor[a].position.x, bigMeteor[a].position.y };
                                    mediumMeteor[midMeteorsCount].speed = Vector2{ (float)cos(shoot[i].rotation * DEG2RAD) * METEORS_SPEED, (float)sin(shoot[i].rotation * DEG2RAD) * METEORS_SPEED };
                                }

                                mediumMeteor[midMeteorsCount].active = true;
                                midMeteorsCount++;
                            }
                            
                            a = MAX_BIG_METEORS;
                        }
                    }

                    for (int b = 0; b < MAX_MEDIUM_METEORS; b++)
                    {
                        if (mediumMeteor[b].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, mediumMeteor[b].position, mediumMeteor[b].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            mediumMeteor[b].active = false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j++)
                            {
                                if (smallMeteorsCount % 2 == 0)
                                {
                                    smallMeteor[smallMeteorsCount].position = Vector2{ mediumMeteor[b].position.x, mediumMeteor[b].position.y };
                                    smallMeteor[smallMeteorsCount].speed = Vector2{ (float)cos(shoot[i].rotation * DEG2RAD) * METEORS_SPEED * -1, (float)sin(shoot[i].rotation * DEG2RAD) * METEORS_SPEED * -1 };
                                }
                                else
                                {
                                    smallMeteor[smallMeteorsCount].position = Vector2{ mediumMeteor[b].position.x, mediumMeteor[b].position.y };
                                    smallMeteor[smallMeteorsCount].speed = Vector2{ (float)cos(shoot[i].rotation * DEG2RAD) * METEORS_SPEED, (float)sin(shoot[i].rotation * DEG2RAD) * METEORS_SPEED };
                                }

                                smallMeteor[smallMeteorsCount].active = true;
                                smallMeteorsCount++;
                            }
                            
                            b = MAX_MEDIUM_METEORS;
                        }
                    }

                    for (int c = 0; c < MAX_SMALL_METEORS; c++)
                    {
                        if (smallMeteor[c].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, smallMeteor[c].position, smallMeteor[c].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            smallMeteor[c].active = false;
                            destroyedMeteorsCount++;
                            
                            c = MAX_SMALL_METEORS;
                        }
                    }
                }
            }
        }

        if (destroyedMeteorsCount == MAX_BIG_METEORS + MAX_MEDIUM_METEORS + MAX_SMALL_METEORS) victory = true;
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame()
{
    BeginDrawing();

    ClearBackground(BLACK);

    //Mouse position
    DrawCircle(GetMouseX(), GetMouseY(), 5, RED);

    if (!gameOver)
    {
        // Draw spaceship
        DrawTexturePro(
            player.sprite,
            Rectangle{ 0,0,(float)player.sprite.width,(float)player.sprite.height },
            Rectangle{ player.position.x, player.position.y, (float)player.sprite.width * 0.3f,(float)player.sprite.height * 0.3f },
            Vector2{ ((float)player.sprite.width * 0.3f) / 2, ((float)player.sprite.height * 0.3f) / 2 },
            player.rotation,
            WHITE);

        // Draw collision
#if _DEBUG
        DrawCircle(player.position.x, player.position.y, 18.0f, Fade(GREEN, 0.5f));
#endif // _DEBUG



        // Draw meteors
        for (int i = 0; i < MAX_BIG_METEORS; i++)
        {
            if (bigMeteor[i].active)
            {
                DrawTexturePro(
                    bigMeteor[i].sprite,
                    Rectangle{ 0,0,(float)bigMeteor[i].sprite.width,(float)bigMeteor[i].sprite.height },
                    Rectangle{ bigMeteor[i].position.x, bigMeteor[i].position.y, (float)bigMeteor[i].sprite.width * 0.9f,(float)bigMeteor[i].sprite.height * 0.9f },
                    Vector2{ ((float)bigMeteor[i].sprite.width * 0.9f) / 2, ((float)bigMeteor[i].sprite.height * 0.9f) / 2 },
                    bigMeteor[i].rotation,
                    WHITE);
#if _DEBUG
                DrawCircleV(bigMeteor[i].position, bigMeteor[i].radius, Fade(RED, 0.5f));
#endif // _DEBUG
            }
        }

        for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
        {
            if (mediumMeteor[i].active)
            {

                DrawTexturePro(
                    mediumMeteor[i].sprite,
                    Rectangle{ 0,0,(float)mediumMeteor[i].sprite.width,(float)mediumMeteor[i].sprite.height },
                    Rectangle{ mediumMeteor[i].position.x, mediumMeteor[i].position.y, (float)mediumMeteor[i].sprite.width * 0.45f,(float)mediumMeteor[i].sprite.height * 0.45f },
                    Vector2{ ((float)mediumMeteor[i].sprite.width * 0.45f) / 2, ((float)mediumMeteor[i].sprite.height * 0.45f) / 2 },
                    mediumMeteor[i].rotation,
                    WHITE);

#if _DEBUG
                DrawCircleV(mediumMeteor[i].position, mediumMeteor[i].radius, Fade(RED, 0.5f));
#endif // _DEBUG
            }
        }

        for (int i = 0; i < MAX_SMALL_METEORS; i++)
        {
            if (smallMeteor[i].active)
            {

                DrawTexturePro(
                    smallMeteor[i].sprite,
                    Rectangle{ 0,0,(float)smallMeteor[i].sprite.width,(float)smallMeteor[i].sprite.height },
                    Rectangle{ smallMeteor[i].position.x, smallMeteor[i].position.y, (float)smallMeteor[i].sprite.width * 0.25f,(float)smallMeteor[i].sprite.height * 0.25f },
                    Vector2{ ((float)smallMeteor[i].sprite.width * 0.25f) / 2, ((float)smallMeteor[i].sprite.height * 0.25f) / 2 },
                    smallMeteor[i].rotation,
                    WHITE);

#if _DEBUG
                DrawCircleV(smallMeteor[i].position, smallMeteor[i].radius, Fade(RED, 0.5f));
#endif // _DEBUG
            }
        }

        // Draw shoot
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
        {
            if (shoot[i].active) DrawCircleV(shoot[i].position, shoot[i].radius, WHITE);
        }

        // Shield Bar
        DrawRectangleRoundedLines(Rectangle{ 10,10,300,20 }, 1, 12, 5, DARKBLUE);
        DrawRectangleRounded(Rectangle{ 10,10, ((float)player.shield/PLAYER_MAX_SHIELD) * 300,20 }, 1, 12, SKYBLUE);

        if (victory) DrawText("VICTORY", screenWidth / 2 - MeasureText("VICTORY", 20) / 2, screenHeight / 2, 20, LIGHTGRAY);

        if (pause) DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
    }
    else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame()
{
    UnloadTexture(player.sprite);

    for (int i = 0; i < MAX_BIG_METEORS; i++)
    {
        UnloadTexture(bigMeteor[i].sprite);
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
    {
        UnloadTexture(mediumMeteor[i].sprite);
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++)
    {
        UnloadTexture(smallMeteor[i].sprite);
    }
}

// Update and Draw (one frame)
void UpdateDrawGameFrame()
{
    UpdateGame();
    DrawGame();
}

void Run()
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "AsteroidXD");

    InitGame();

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawGameFrame();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}
