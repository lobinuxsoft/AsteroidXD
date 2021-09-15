#include "GameManager.h"
#include "raylib.h"
#include <cmath>
#include <string>

#pragma region DEFINES

#define PLAYER_BASE_SIZE    18.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   10
#define PLAYER_MAX_SHIELD   100

#define METEORS_SPEED       2
#define MAX_BIG_METEORS     10
#define MAX_MEDIUM_METEORS  2 * MAX_BIG_METEORS
#define MAX_SMALL_METEORS   2 * MAX_MEDIUM_METEORS

#pragma endregion

#pragma region ENUMS

enum class GameState
{
    MainMenu,
    Gameplay,
    Result
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

#pragma region STRUCTS

struct Player
{
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
        // Player logic: speed
        speed.x = sin(rotation * DEG2RAD) * PLAYER_SPEED;
        speed.y = cos(rotation * DEG2RAD) * PLAYER_SPEED;

        // Player logic: acceleration
        if (moving)
        {
            if (acceleration < 1) acceleration += 0.04f;
        }
        else
        {
            if (acceleration > 0) acceleration -= 0.02f;
            else if (acceleration < 0) acceleration = 0;
        }

        // Player logic: movement
        position.x += (speed.x * acceleration);
        position.y -= (speed.y * acceleration);
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
        // Draw spaceship
        DrawTexturePro(
            sprite,
            Rectangle{ 0,0,(float)sprite.width,(float)sprite.height },
            Rectangle{ position.x, position.y, (float)sprite.width * 0.3f,(float)sprite.height * 0.3f },
            Vector2{ ((float)sprite.width * 0.3f) / 2, ((float)sprite.height * 0.3f) / 2 },
            rotation,
            WHITE);

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
            position.x += speed.x;
            position.y += speed.y;
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
    Vector2 position { 0, 0 };
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

#pragma region GLOBAL VARIABLES

static const int screenWidth = 800;
static const int screenHeight = 450;

static GameState gameState = GameState::MainMenu;
static bool gameOver = false;
static bool pause = false;
static bool victory = false;

// Gameplay objects
static float shipRadius = 0;

static Player player;
static Shoot shoot[PLAYER_MAX_SHOOTS];
static Meteor bigMeteor[MAX_BIG_METEORS];
static Meteor mediumMeteor[MAX_MEDIUM_METEORS];
static Meteor smallMeteor[MAX_SMALL_METEORS];

static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;

// HUD
static ProgressBar shieldBar;
static Button playButton;

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

#pragma region HUD and Main Menu

    // Initialice menu buttons
    playButton.text = "PLAY";
    playButton.fontSize = 40;
    playButton.textColor = WHITE;
    playButton.normalColor = BLUE;
    playButton.pressColor = SKYBLUE;
    playButton.position = Vector2{ (float)GetScreenWidth() * 0.4f, (float)GetScreenHeight() * 0.4f };
    playButton.hMargin = 10;
    playButton.vMargin = 10;

    // Initialization Shield bar
    shieldBar.position = Vector2{ 10,10 };
    shieldBar.width = 300;
    shieldBar.height = 10;
    shieldBar.front = BLUE;
    shieldBar.back = DARKBLUE;

#pragma endregion

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
    switch (gameState)
    {
    case GameState::MainMenu:

        // Play button behaviour
        playButton.MouseCollision();

        if (playButton.click)
        {
            // TODO modificar como funcionan los meteoros
            InitGame();
            gameState = GameState::Gameplay;
        }

        break;
    case GameState::Gameplay:

        if (!gameOver)
        {
            if (IsKeyPressed('P')) pause = !pause;

            if (!pause)
            {
                player.LookAt(GetMousePosition());

                player.MoveForward(IsMouseButtonDown(1));

                player.ScreenLimitsLogic(screenWidth, screenHeight);

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
                    bigMeteor[i].Movement();
                    bigMeteor[i].ScreenLimitsLogic(screenWidth, screenHeight);
                }

                // Meteors logic: medium meteors
                for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
                {
                    mediumMeteor[i].Movement();
                    mediumMeteor[i].ScreenLimitsLogic(screenWidth, screenHeight);
                }

                // Meteors logic: small meteors
                for (int i = 0; i < MAX_SMALL_METEORS; i++)
                {
                    smallMeteor[i].Movement();
                    smallMeteor[i].ScreenLimitsLogic(screenWidth, screenHeight);
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

        break;
    case GameState::Result:
        // TODO result logic
        break;
    }
}

// Draw game (one frame)
void DrawGame()
{
    BeginDrawing();

    ClearBackground(BLACK);

    switch (gameState)
    {
    case GameState::MainMenu:

        DrawText("Asteroid XD", GetScreenWidth() / 2 - MeasureText("Asteroid XD", 40) / 2, GetScreenHeight() * 0.15f, 40, WHITE);
        DrawText("Created by Matias Galarza (art from Kenney)", GetScreenWidth() * 0.01f, GetScreenHeight() - 15, 15, GRAY);

        playButton.text = "PLAY";
        playButton.fontSize = 40;
        playButton.textColor = WHITE;
        playButton.normalColor = BLUE;
        playButton.pressColor = SKYBLUE;
        playButton.position = Vector2{ (float)GetScreenWidth() * 0.4f, (float)GetScreenHeight() * 0.4f };
        playButton.hMargin = 10;
        playButton.vMargin = 10;

        playButton.Draw();

        break;

    case GameState::Gameplay:
        if (!gameOver)
        {

            player.Draw();

            // Draw meteors
            for (int i = 0; i < MAX_BIG_METEORS; i++)
            {
                bigMeteor[i].Draw();
            }

            for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
            {
                mediumMeteor[i].Draw();
            }

            for (int i = 0; i < MAX_SMALL_METEORS; i++)
            {
                smallMeteor[i].Draw();
            }

            // Draw shoot
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                shoot[i].Draw();
            }

            // Shield Bar
            shieldBar.Draw((float)player.shield / PLAYER_MAX_SHIELD);

            if (victory) DrawText("VICTORY", screenWidth / 2 - MeasureText("VICTORY", 20) / 2, screenHeight / 2, 20, LIGHTGRAY);

            if (pause) DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

        break;
    case GameState::Result:
        break;
    }

    //Mouse position
    DrawCircle(GetMouseX(), GetMouseY(), 5, RED);

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
