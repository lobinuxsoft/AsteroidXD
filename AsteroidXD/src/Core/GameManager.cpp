#include "GameManager.h"
#include "raylib.h"
#include <string>
#include <vector>
#include "Class/GameObjects/Ship.h"
#include "Class/GameObjects/Meteor.h"
#include "Class/GameObjects/Shoot.h"
#include "Class/UIObjects/Button.h"
#include "Class/UIObjects/ProgressBar.h"

#pragma region CONSTANT VARIABLES

static const char gameIconUrl[] = "resources/images/icon.png";
static Image gameIcon;
static const int screenWidth = 800;
static const int screenHeight = 450;
static bool isFullScreen = false;

static const int shipMaxShoots = 10;
static const int shipMaxShield = 100;
static const float shipRadius = 18.0f;

static const float meteorsSpeed = 150.0f;

#pragma endregion

#pragma region ENUMS

enum class GameState
{
    MainMenu,
    Gameplay,
    Credits
};

#pragma endregion


#pragma region GLOBAL VARIABLES

static GameState gameState = GameState::MainMenu;
static bool gameOver = false;
static bool pause = false;
static bool victory = false;
static int level = 1;

// Gameplay objects
// Player Ship and shoots---------------------------
static const char shipImgUrl[] = "resources/images/ship_G.png";
static Ship *player;
static std::vector<Shoot*> shoot;
//--------------------------------------------------

// Meteors------------------------------------------
static const char meteorImgUrl[] = "resources/images/meteor_detailedLarge.png";
static std::vector<Meteor*> bigMeteor;
static std::vector<Meteor*> mediumMeteor;
static std::vector<Meteor*> smallMeteor;

static const int baseAmountBigMeteors = 4;
static int maxBigMeteors = baseAmountBigMeteors;
static int maxMediumMeteors = maxBigMeteors * 2;
static int maxSmallMeteors = maxMediumMeteors * 2;

static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;
//-------------------------------------------------

// HUD
// Main menu---------------------------------------
static Button* playButton;
//-------------------------------------------------

// Gameplay----------------------------------------
static ProgressBar* shieldBar;
static Button* pauseButton;
static Button* reTryButton;
static Button* returnMenuButton;
//------------------------------------------------

#pragma endregion

// Initialize game variables
static void InitGame()
{
    float posx, posy;
    float velx, vely;
    bool correctRange = false;
    gameOver = false;
    victory = false;
    pause = false;

    // Initialization player
    if(player == nullptr)
    {
        player = new Ship(Vector2{ GetScreenWidth() / 2 - shipRadius / 2, GetScreenHeight() / 2 - shipRadius / 2 }, shipImgUrl);
    }
    else
    {
        player->setPosition(Vector2{ GetScreenWidth() / 2 - shipRadius / 2, GetScreenHeight() / 2 - shipRadius / 2 });
        player->resetState();
    }

#pragma region HUD and Main Menu

    // Initialice menu buttons
    if (playButton == nullptr)
    {
        playButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f },
            "PLAY", 20, 40, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        playButton->setPivot({ 0.5f, 0.5f });
    }
    else
    {
        playButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f });
    }

    if (pauseButton == nullptr)
    {
        pauseButton = new Button
        (
            Vector2{ (float)GetScreenWidth() - 20, 10 },
            " || ", 20, 5, 1, 0.5f, 16, 3, WHITE, RED, MAROON
        );

        pauseButton->setPivot({ 1,0 });
    }
    else
    {
        pauseButton->setPosition(Vector2{ (float)GetScreenWidth() - 20, 10 });
    }

    if(reTryButton == nullptr)
    {
        reTryButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.6f },
            "RE-INTENTAR", 20, 10, 10, 1, 16, 3, WHITE, DARKGREEN, GREEN
        );

        reTryButton->setPivot({ 0.5f, 0.5f });
    }
    else
    {
        reTryButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.6f });
    }

    if (returnMenuButton == nullptr)
    {
        returnMenuButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.75f },
            "VOLVER AL MENU", 20, 10, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        returnMenuButton->setPivot({ 0.5f, 0.5f });
    }
    else
    {
        returnMenuButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.75f });
    }

    // Initialization Shield bar
    if (shieldBar == nullptr)
    {
        shieldBar = new ProgressBar(Vector2{ 20,10 }, 300, 10, 1, 12, 2);
    }


#pragma endregion

    destroyedMeteorsCount = 0;

    for (Shoot* s : shoot)
    {
        delete s;
    }
    shoot.clear();

    // Initialization shoot
    for (int i = 0; i < shipMaxShoots; i++)
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

    maxBigMeteors = baseAmountBigMeteors * level;
    maxMediumMeteors = maxBigMeteors * 2;
    maxSmallMeteors = maxMediumMeteors * 2;

    for (int i = 0; i < maxBigMeteors; i++)
    {
        posx = GetRandomValue(0, GetScreenWidth());

        while (!correctRange)
        {
            if (posx > GetScreenWidth() / 2 - 150 && posx < GetScreenWidth() / 2 + 150) posx = GetRandomValue(0, GetScreenWidth());
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, GetScreenHeight());

        while (!correctRange)
        {
            if (posy > GetScreenHeight() / 2 - 150 && posy < GetScreenHeight() / 2 + 150)  posy = GetRandomValue(0, GetScreenHeight());
            else correctRange = true;
        }


        correctRange = false;
        velx = GetRandomValue(-meteorsSpeed, meteorsSpeed);
        vely = GetRandomValue(-meteorsSpeed, meteorsSpeed);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-meteorsSpeed, meteorsSpeed);
                vely = GetRandomValue(-meteorsSpeed, meteorsSpeed);
            }
            else correctRange = true;
        }

        bigMeteor.push_back(new Meteor(Vector2{ posx, posy }, meteorImgUrl, Vector2{ velx, vely }, meteorsSpeed, GetRandomValue(0, 360), 40, true));
    }

    for (int i = 0; i < maxMediumMeteors; i++)
    {
        mediumMeteor.push_back(new Meteor(Vector2{ -100, -100 }, meteorImgUrl, Vector2{ 0,0 }, meteorsSpeed, GetRandomValue(0, 360), 20, false));
    }

    for (int i = 0; i < maxSmallMeteors; i++)
    {
        smallMeteor.push_back(new Meteor(Vector2{ -100, -100 }, meteorImgUrl, Vector2{ 0,0 }, meteorsSpeed, GetRandomValue(0, 360), 10, false));
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;

    HideCursor();
}

// Update game (one frame)
static void UpdateGame()
{
    switch (gameState)
    {
    case GameState::MainMenu:

        // Play button behaviour
        playButton->update();

        if (playButton->isClick())
        {
            InitGame();
            gameState = GameState::Gameplay;
        }

        break;
    case GameState::Gameplay:

        if (!gameOver)
        {
            if (!victory)
            {
                pauseButton->update();
                if (pauseButton->isClick()) pause = !pause;
            }
            else
            {
                reTryButton->update();
                returnMenuButton->update();

                if (reTryButton->isClick())
                {
                    level++;
                    InitGame();
                }

                if (returnMenuButton->isClick()) gameState = GameState::MainMenu;
            }

            if (!pause)
            {
                player->update();

                // Player shoot logic
                if (IsMouseButtonPressed(0))
                {
                    for (int i = 0; i < shipMaxShoots; i++)
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
                            shoot[i]->setSpeed(player->getRotation(), player->getMaxSpeed());
                            break;
                        }
                    }
                }

                // Shoot life timer
                for (int i = 0; i < shipMaxShoots; i++)
                {
                    if (shoot[i]->getActive()) shoot[i]->addLifeSpawn();
                }

                // Shot logic
                for (int i = 0; i < shipMaxShoots; i++)
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
                for (int i = 0; i < shipMaxShoots; i++)
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
            else
            {
                reTryButton->update();
                if (reTryButton->isClick())
                {
                    InitGame();
                }

                returnMenuButton->update();
                if (returnMenuButton->isClick()) gameState = GameState::MainMenu;
            }

            if (destroyedMeteorsCount == maxBigMeteors + maxMediumMeteors + maxSmallMeteors) victory = true;
        }
        else 
        {
            reTryButton->update();
            returnMenuButton->update();

            if (reTryButton->isClick()) InitGame();
            if (returnMenuButton->isClick()) gameState = GameState::MainMenu;
        }

        break;

    case GameState::Credits:
        // TODO credits
        break;
    }
}

// Draw game (one frame)
static void DrawGame()
{
    BeginDrawing();

    ClearBackground(BLACK);

    switch (gameState)
    {
    case GameState::MainMenu:

        DrawText("Asteroid xD", (float)GetScreenWidth() / 2 - MeasureText("Asteroid xD", 60) / 2, (float)GetScreenHeight() * 0.15f, 60, WHITE);
        DrawText("Created by Matias Galarza (art from Kenney)", GetScreenWidth() * 0.01f, GetScreenHeight() - 15, 15, GRAY);

        playButton->setText(TextFormat("JUGAR NIVEL %0i", level));
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
            for (int i = 0; i < shipMaxShoots; i++)
            {
                shoot[i]->draw();
            }

            // Shield Bar
            shieldBar->setProgressValue((float)player->getShield() / shipMaxShield);
            shieldBar->draw();

            if (victory)
            {
                DrawText(TextFormat("NIVEL %0i COMPLETADO", level), screenWidth / 2 - MeasureText(TextFormat("NIVEL %0i COMPLETADO", level), 40) / 2, screenHeight * 0.25f, 40, LIGHTGRAY);
                reTryButton->setText("SIGUIENTE NIVEL");
                reTryButton->draw();
                returnMenuButton->draw();
            }
            else
            {
                pauseButton->draw();

                if (pause) 
                {
                    DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight * 0.25f, 40, GRAY);
                    
                    reTryButton->setText("REINICIAR NIVEL");
                    reTryButton->draw();
                    returnMenuButton->draw();
                } 
            }
        }
        else
        {
            DrawText("FIN DEL JUEGO", GetScreenWidth() / 2 - MeasureText("FIN DEL JUEGO", 40) / 2, GetScreenHeight() * 0.25f, 40, GRAY);
            DrawText(TextFormat("llegaste hasta el nivel %0i", level), GetScreenWidth() / 2 - MeasureText(TextFormat("llegaste hasta el nivel %0i", level), 40) / 2, GetScreenHeight() * 0.35f, 40, GRAY);
            reTryButton->setText("NO ME RINDO");
            reTryButton->draw();
            returnMenuButton->draw();
        }

        break;

    case GameState::Credits:
        break;
    }

    //Mouse position
    DrawCircle(GetMouseX(), GetMouseY(), 5, RED);

    EndDrawing();
}

// Unload game variables
static void UnloadGame()
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
    delete reTryButton;
    delete returnMenuButton;

    UnloadImage(gameIcon);
}

// Update and Draw (one frame)
static void UpdateDrawGameFrame()
{
    UpdateGame();
    DrawGame();
}

void Run()
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Asteroid xD");
    gameIcon = LoadImage(gameIconUrl);
    SetWindowIcon(gameIcon);

    

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
