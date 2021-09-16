#include "GameManager.h"
#include "raylib.h"
#include <string>
#include <vector>
#include "../Class/GameObjects/Ship.h"
#include "../Class/GameObjects/Meteor.h"
#include "../Class/GameObjects/Shoot.h"
#include "../Class/UIObjects/Button.h"
#include "../Class/UIObjects/ProgressBar.h"

#pragma region DEFINES

#define PLAYER_BASE_SIZE    18.0f
#define PLAYER_SPEED        150.0f
#define PLAYER_MAX_SHOOTS   10
#define PLAYER_MAX_SHIELD   100

#define METEORS_SPEED       150.0f

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

//#pragma region STRUCTS
//
//struct ProgressBar
//{
//    Vector2 position { 0, 0 };
//    float width = 300;
//    float height = 10;
//    Color front = BLUE;
//    Color back = DARKBLUE;
//
//    void Draw(float value = 1.0f)
//    {
//        // Shield Bar
//        DrawRectangleRoundedLines(Rectangle{ position.x,position.y, width, height }, 1, 12, 5, back);
//        DrawRectangleRounded(Rectangle{ position.x, position.y, value * width, height }, 1, 12, front);
//    }
//};
//
//#pragma endregion

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
static std::vector<Shoot*> shoot;
static std::vector<Meteor*> bigMeteor;
static std::vector<Meteor*> mediumMeteor;
static std::vector<Meteor*> smallMeteor;

static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;

// HUD
static ProgressBar *shieldBar;
static Button *playButton;

// Meteors
int maxBigMeteors = 4;
int maxMediumMeteors = maxBigMeteors * 2;
int maxSmallMeteors = maxMediumMeteors * 2;

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
    if (playButton == nullptr)
    {
        playButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.4f, (float)GetScreenHeight() * 0.4f },
            "PLAY", 40, WHITE, BLUE, SKYBLUE, 10, 10, 1
        );
    }

    // Initialization Shield bar
    if (shieldBar == nullptr)
    {
        shieldBar = new ProgressBar(Vector2{ 10,10 }, 300, 10, 1, 12, 2);
    }

#pragma endregion

    destroyedMeteorsCount = 0;

    for (Shoot* s : shoot)
    {
        delete s;
    }
    shoot.clear();

    // Initialization shoot
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot.push_back(new Shoot(Vector2{ 0, 0 }, Vector2{ 0, 0 }, 2, 0, 0, false, WHITE));
    }

    // Reset all arrays of poiters
    for (Meteor* m : bigMeteor)
    {
        delete m;
    }

    bigMeteor.clear();

    for (Meteor* m : mediumMeteor)
    {
        delete m;
    }

    mediumMeteor.clear();
    
    for (Meteor* m : smallMeteor)
    {
        delete m;
    }

    smallMeteor.clear();
    //----------------------------

    for (int i = 0; i < maxBigMeteors; i++)
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

        bigMeteor.push_back(new Meteor(Vector2{ posx, posy }, "resources/images/meteor_detailedLarge.png", Vector2{ velx, vely }, METEORS_SPEED, GetRandomValue(0, 360), 40, true));
    }

    for (int i = 0; i < maxMediumMeteors; i++)
    {
        mediumMeteor.push_back(new Meteor(Vector2{ -100, -100 }, "resources/images/meteor_detailedLarge.png", Vector2{ 0,0 }, METEORS_SPEED, GetRandomValue(0, 360), 20, false));
    }

    for (int i = 0; i < maxSmallMeteors; i++)
    {
        smallMeteor.push_back(new Meteor(Vector2{ -100, -100 }, "resources/images/meteor_detailedLarge.png", Vector2{ 0,0 }, METEORS_SPEED, GetRandomValue(0, 360), 10, false));
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
        playButton->update();

        if (playButton->isClick())
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

                // Player shoot logic
                if (IsMouseButtonPressed(0))
                {
                    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                    {
                        if (!shoot[i]->getActive())
                        {
                            shoot[i]->setPosition(
                                Vector2
                                {
                                    player->getPosition().x + (float)sin(player->getRotation() * DEG2RAD) * player->getRadius(),
                                    player->getPosition().y - (float)cos(player->getRotation() * DEG2RAD) * player->getRadius()
                                }
                            );


                            shoot[i]->setActive(true);
                            shoot[i]->setSpeed(player->getRotation(), PLAYER_SPEED);
                            break;
                        }
                    }
                }

                // Shoot life timer
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (shoot[i]->getActive()) shoot[i]->addLifeSpawn();
                }

                // Shot logic
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (shoot[i]->getActive())
                    {
                        shoot[i]->update();
                    }
                }

                for (int a = 0; a < maxBigMeteors; a++)
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), bigMeteor[a]->getPosition(), bigMeteor[a]->getRadius()) && bigMeteor[a]->getActive())
                    {
                        gameOver = player->damageShip(bigMeteor[a]->getPosition());
                    }
                }

                for (int a = 0; a < maxMediumMeteors; a++)
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), mediumMeteor[a]->getPosition(), mediumMeteor[a]->getRadius()) && mediumMeteor[a]->getActive())
                    {
                        gameOver = player->damageShip(mediumMeteor[a]->getPosition());
                    }
                }

                for (int a = 0; a < maxSmallMeteors; a++)
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), smallMeteor[a]->getPosition(), smallMeteor[a]->getRadius()) && smallMeteor[a]->getActive())
                    {
                        gameOver = player->damageShip(smallMeteor[a]->getPosition());
                    }
                }

                // Meteors logic: big meteors
                for (int i = 0; i < maxBigMeteors; i++)
                {
                    bigMeteor[i]->update();
                }

                // Meteors logic: medium meteors
                for (int i = 0; i < maxMediumMeteors; i++)
                {
                    mediumMeteor[i]->update();
                }

                // Meteors logic: small meteors
                for (int i = 0; i < maxSmallMeteors; i++)
                {
                    smallMeteor[i]->update();
                }

                // Collision logic: player-shoots vs meteors
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if ((shoot[i]->getActive()))
                    {
                        for (int a = 0; a < maxBigMeteors; a++)
                        {
                            if (bigMeteor[a]->getActive() && CheckCollisionCircles(shoot[i]->getPosition(), shoot[i]->getRadius(), bigMeteor[a]->getPosition(), bigMeteor[a]->getRadius()))
                            {
                                shoot[i]->setActive(false);
                                shoot[i]->resetLifeSpawn();
                                bigMeteor[a]->setActive(false);
                                destroyedMeteorsCount++;

                                for (int j = 0; j < 2; j++)
                                {
                                    if (midMeteorsCount % 2 == 0)
                                    {
                                        mediumMeteor[midMeteorsCount]->setPosition(bigMeteor[a]->getPosition());
                                        mediumMeteor[midMeteorsCount]->setSpeedByAngle(shoot[i]->getRotation(), true);
                                    }
                                    else
                                    {
                                        mediumMeteor[midMeteorsCount]->setPosition(bigMeteor[a]->getPosition());
                                        mediumMeteor[midMeteorsCount]->setSpeedByAngle(shoot[i]->getRotation());
                                    }

                                    mediumMeteor[midMeteorsCount]->setActive(true);
                                    midMeteorsCount++;
                                }

                                a = maxBigMeteors;
                            }
                        }

                        for (int b = 0; b < maxMediumMeteors; b++)
                        {
                            if (mediumMeteor[b]->getActive() && CheckCollisionCircles(shoot[i]->getPosition(), shoot[i]->getRadius(), mediumMeteor[b]->getPosition(), mediumMeteor[b]->getRadius()))
                            {
                                shoot[i]->setActive(false);
                                shoot[i]->resetLifeSpawn();
                                mediumMeteor[b]->setActive(false);
                                destroyedMeteorsCount++;

                                for (int j = 0; j < 2; j++)
                                {
                                    if (smallMeteorsCount % 2 == 0)
                                    {
                                        smallMeteor[smallMeteorsCount]->setPosition(mediumMeteor[b]->getPosition());
                                        smallMeteor[smallMeteorsCount]->setSpeedByAngle(shoot[i]->getRotation(), true);
                                    }
                                    else
                                    {
                                        smallMeteor[smallMeteorsCount]->setPosition(mediumMeteor[b]->getPosition());
                                        smallMeteor[smallMeteorsCount]->setSpeedByAngle(shoot[i]->getRotation());
                                    }

                                    smallMeteor[smallMeteorsCount]->setActive(true);
                                    smallMeteorsCount++;
                                }

                                b = maxMediumMeteors;
                            }
                        }

                        for (int c = 0; c < maxSmallMeteors; c++)
                        {
                            if (smallMeteor[c]->getActive() && CheckCollisionCircles(shoot[i]->getPosition(), shoot[i]->getRadius(), smallMeteor[c]->getPosition(), smallMeteor[c]->getRadius()))
                            {
                                shoot[i]->setActive(false);
                                shoot[i]->resetLifeSpawn();
                                smallMeteor[c]->setActive(false);
                                destroyedMeteorsCount++;

                                c = maxSmallMeteors;
                            }
                        }
                    }
                }
            }

            if (destroyedMeteorsCount == maxBigMeteors + maxMediumMeteors + maxSmallMeteors) victory = true;
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

        playButton->draw();

        break;

    case GameState::Gameplay:
        if (!gameOver)
        {

            player->draw();

            // Draw meteors
            for (int i = 0; i < maxBigMeteors; i++)
            {
                bigMeteor[i]->draw();
            }

            for (int i = 0; i < maxMediumMeteors; i++)
            {
                mediumMeteor[i]->draw();
            }

            for (int i = 0; i < maxSmallMeteors; i++)
            {
                smallMeteor[i]->draw();
            }

            // Draw shoot
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                shoot[i]->draw();
            }

            // Shield Bar
            shieldBar->setProgressValue((float)player->getShield() / PLAYER_MAX_SHIELD);
            shieldBar->draw();

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
    // Delete Player
    delete player;

    // Delete Shoots
    for (Shoot* s : shoot)
    {
        delete s;
    }
    shoot.clear();

    // Delete Meteors
    for (Meteor *m : bigMeteor)
    {
        delete m;
    }
    bigMeteor.clear();

    for (Meteor *m : mediumMeteor)
    {
        delete m;
    }
    mediumMeteor.clear();

    for (Meteor *m : smallMeteor)
    {
        delete m;
    }
    smallMeteor.clear();

    // Delete UI
    delete playButton;
    delete shieldBar;
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
