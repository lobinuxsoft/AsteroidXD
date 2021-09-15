#include "GameManager.h"
#include "raylib.h"
#include <string>
#include "../Class/Ship.h"

#pragma region DEFINES

#define PLAYER_BASE_SIZE    18.0f
#define PLAYER_SPEED        150.0f
#define PLAYER_MAX_SHOOTS   10
#define PLAYER_MAX_SHIELD   100

#define METEORS_SPEED       150.0f
#define MAX_BIG_METEORS     10
#define MAX_MEDIUM_METEORS  2 * MAX_BIG_METEORS
#define MAX_SMALL_METEORS   2 * MAX_MEDIUM_METEORS

#pragma endregion

#pragma region ENUMS

enum class GameState
{
    MainMenu,
    Credits,
    Gameplay,
    Result
};

#pragma endregion

#pragma region STRUCTS

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

static Ship *player;
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
    if(player == nullptr)
    {
        player = new Ship(Vector2{ screenWidth / 2 - shipRadius / 2, screenHeight / 2 - shipRadius / 2 },"resources/images/ship_G.png", PLAYER_SPEED, PLAYER_MAX_SHIELD, shipRadius);
    }
    else
    {
        player->setPosition(Vector2{ screenWidth / 2 - shipRadius / 2, screenHeight / 2 - shipRadius / 2 });
        player->setShield(PLAYER_MAX_SHIELD);
    }

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
                player->update();
                player->screenLimitsLogic(screenWidth, screenHeight);

                // Player shoot logic
                if (IsMouseButtonPressed(0))
                {
                    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                    {
                        if (!shoot[i].active)
                        {
                            shoot[i].position = Vector2
                            { 
                                player->getPosition().x + (float)sin(player->getRotation() * DEG2RAD) * player->getRadius(), 
                                player->getPosition().y - (float)cos(player->getRotation() * DEG2RAD) * player->getRadius()
                            };

                            shoot[i].active = true;
                            shoot[i].speed.x = 1.5 * sin(player->getRotation() * DEG2RAD) * PLAYER_SPEED * GetFrameTime();
                            shoot[i].speed.y = 1.5 * cos(player->getRotation() * DEG2RAD) * PLAYER_SPEED * GetFrameTime();
                            shoot[i].rotation = player->getRotation();
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
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active)
                    {
                        gameOver = player->damageShip(bigMeteor[a].position);
                    }
                }

                for (int a = 0; a < MAX_MEDIUM_METEORS; a++)
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), mediumMeteor[a].position, mediumMeteor[a].radius) && mediumMeteor[a].active)
                    {
                        gameOver = player->damageShip(mediumMeteor[a].position);
                    }
                }

                for (int a = 0; a < MAX_SMALL_METEORS; a++)
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), smallMeteor[a].position, smallMeteor[a].radius) && smallMeteor[a].active)
                    {
                        gameOver = player->damageShip(smallMeteor[a].position);
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

            player->draw();

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
            shieldBar.Draw((float)player->getShield() / PLAYER_MAX_SHIELD);

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
    delete player;

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
