#include "GameManager.h"
#include "raylib.h"
#include <string>
#include <vector>
#include "Class/GameObjects/Ship.h"
#include "Class/GameObjects/Meteor.h"
#include "Class/GameObjects/Shoot.h"
#include "Class/UIObjects/Button.h"
#include "Class/UIObjects/ProgressBar.h"
#include "Class/GameObjects/HpPowerUp.h"

#pragma region CONSTANT VARIABLES

static const char gameIconUrl[] = "resources/images/icon.png";
static Image gameIcon;
static const int screenWidth = 1024;
static const int screenHeight = 720;
static bool isFullScreen = false;

static const int shipMaxShoots = 10;
static const int shipMaxShield = 100;
static const float shipRadius = 18.0f;

static const float meteorsSpeed = 150.0f;

static const char tutorialText[] = R"(
Destruye todos los asteroides para superar el nivel,
si los asteroides tocan la nave se reduce su integridad.

El medidor de integridad se encuentra en la esquina superior izquierda,
cuando esta se vacie el juego se termina.

Puedes pausar el juego si haces CLICK en el boton de pausa,
que se encuentra en la esquina superior derecha.

Comportamiento de la nave:
- Siempre mira donde esta el puntero del mouse.
- Boton derecho del mouse para acelerar.
- Boton izquierdo del mouse para disparar.

DISFRUTA EL JUEGO xD
)";


static const char creditsText[] = R"(
Desarrollador: Matias Galarza

Arte: 
-Simple Space, creado por Kenney (https://kenney.nl/assets/simple-space)
-Crosshair Pack, creado por Kenney (https://kenney.nl/assets/crosshair-pack)
-Space Background, creado por Ansimuz (https://opengameart.org/content/space-background-3)

Sounds FX:
- Sci-Fi Sounds, creado por Kenney (https://kenney.nl/assets/sci-fi-sounds)
- Interface Sounds, creado por Kenney (https://kenney.nl/assets/interface-sounds)

Musica:
- Cyberpunk Moonlight Sonata,
  creado por Joth (https://opengameart.org/content/cyberpunk-moonlight-sonata)

- Fight creado por Ville Nousiainen / Xythe / mutkanto
  (https://opengameart.org/content/fast-fight-battle-music)
)";

#pragma endregion

#pragma region ENUMS

enum class GameState
{
    MainMenu,
    Gameplay,
    Tutorial,
    Credits,
    Quit
};

#pragma endregion

#pragma region GLOBAL VARIABLES

static GameState gameState = GameState::MainMenu;
static bool gameOver = false;
static bool pause = false;
static bool victory = false;
static int level = 1;

static const char crosshair1Url[] = "resources/images/crosshair158.png";
static const char crosshair2Url[] = "resources/images/crosshair185.png";

static Texture2D crosshair1;
static Texture2D crosshair2;
static float crosshair1Rot = 0.0f;
static float crosshair2Rot = 0.0f;

// Backgrounds--------------------------------------

static const char backgroundUrl[] = "resources/images/parallax-space-far-planets.png";
static const char midgroundUrl[] = "resources/images/parallax-space-stars.png";
static Texture2D background;
static Texture2D midground;
static float scrollingBack = 0.0f;
static float scrollingMid = 0.0f;

//--------------------------------------------------

// Music--------------------------------------------

static const char mainMusicUrl[] = "resources/music/CyberpunkMoonlightSonata.ogg";
static const char gameplayMusicUrl[] = "resources/music/fight.ogg";

static Music mainMusic;
static Music gameplayMusic;

//--------------------------------------------------

// Gameplay objects
// Player Ship and shoots---------------------------
static const char shipImgUrl[] = "resources/images/ship_G.png";
static Ship* player;
static const char engineSfxUrl[] = "resources/sfx/engineCircular_000.ogg";
static const char shieldSfxUrl[] = "resources/sfx/forceField_000.ogg";
static const char explodeShipSfxUrl[] = "resources/sfx/explosionCrunch_004.ogg";
static std::vector<Shoot*> shoot;
static const char laserSfxUrl[] = "resources/sfx/laserLarge_000.ogg";
static Sound laserSfx;
static HpPowerUp* hpPowerUp;
static const char hpSpriteUrl[] = "resources/images/icon_plusSmall.png";
static const char hpSfxUrl[] = "resources/sfx/question_003.ogg";
//--------------------------------------------------

// Meteors------------------------------------------
static const char meteorImgUrl[] = "resources/images/meteor_detailedLarge.png";
static const char meteorExplodeSfxUrl[] = "resources/sfx/explosionCrunch_000.ogg";
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
static const char playSfxUrl[] = "resources/sfx/confirmation_002.ogg";
static const char clickSfxUrl[] = "resources/sfx/select_003.ogg";
static const char tutorialSfxUrl[] = "resources/sfx/question_003.ogg";
static Button* playButton;
static Button* tutorialButton;
static Button* creditsButton;
static Button* quitButton;
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

    crosshair1 = LoadTexture(crosshair1Url);
    crosshair2 = LoadTexture(crosshair2Url);

    background = LoadTexture(backgroundUrl);
    midground = LoadTexture(midgroundUrl);

#pragma region Audio

    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();

        mainMusic = LoadMusicStream(mainMusicUrl);
        SetMusicVolume(mainMusic, 0.15f);

        gameplayMusic = LoadMusicStream(gameplayMusicUrl);
        SetMusicVolume(gameplayMusic, 0.15f);

        laserSfx = LoadSound(laserSfxUrl);
        SetSoundVolume(laserSfx, 0.5f);
    }

#pragma endregion

#pragma region Player Shoots and Powerups

    // Initialization player
    if(player == nullptr)
    {
        player = new Ship(Vector2{ GetScreenWidth() / 2 - shipRadius / 2, GetScreenHeight() / 2 - shipRadius / 2 }, shipImgUrl, engineSfxUrl, shieldSfxUrl, explodeShipSfxUrl);
    }
    else
    {
        player->setPosition(Vector2{ GetScreenWidth() / 2 - shipRadius / 2, GetScreenHeight() / 2 - shipRadius / 2 });
        player->resetState();
    }

    if (hpPowerUp == nullptr)
    {
        hpPowerUp = new HpPowerUp({ 0,0 }, hpSpriteUrl, hpSfxUrl, { 0,0 }, meteorsSpeed, 0, shipRadius, false);
    }

    for (Shoot* s : shoot)
    {
        delete s;
    }
    shoot.clear();

#pragma endregion


#pragma region HUD and Main Menu

    // Initialice menu buttons
    if (playButton == nullptr)
    {
        playButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f },
            "PLAY", playSfxUrl, 18, 20, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        playButton->setPivot({ 0.5f, 0.5f });
    }
    else
    {
        playButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f });
    }

    if (tutorialButton == nullptr)
    {
        tutorialButton = new Button
        (
            Vector2{ 0, 0 },
            "TUTORIAL", tutorialSfxUrl, 18, 20, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        tutorialButton->setPivot({ 0.5f, 0.5f });
    }

    if (creditsButton == nullptr)
    {
        creditsButton = new Button
        (
            Vector2{ 0, 0 },
            "CREDITOS", clickSfxUrl, 18, 20, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        creditsButton->setPivot({ 0.5f,0.5f });
    }

    if (quitButton == nullptr)
    {
        quitButton = new Button
        (
            Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.875f },
            "SALIR DEL JUEGO", clickSfxUrl, 18, 20, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
        );

        quitButton->setPivot({ 0.5f,0.5f });
    }
    else
    {
        quitButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.875f });
    }

    if (pauseButton == nullptr)
    {
        pauseButton = new Button
        (
            Vector2{ (float)GetScreenWidth() - 20, 10 },
            " || ", clickSfxUrl, 20, 5, 1, 0.5f, 16, 3, WHITE, RED, MAROON
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
            "RE-INTENTAR", playSfxUrl, 20, 10, 10, 1, 16, 3, WHITE, DARKGREEN, GREEN
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
            "VOLVER AL MENU", clickSfxUrl, 20, 10, 10, 1, 16, 3, WHITE, DARKBLUE, BLUE
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

#pragma region Meteors

    destroyedMeteorsCount = 0;

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

        bigMeteor.push_back(new Meteor(Vector2{ posx, posy }, meteorImgUrl, meteorExplodeSfxUrl, Vector2{ velx, vely }, meteorsSpeed, GetRandomValue(0, 360), 40, true));
    }

    for (int i = 0; i < maxMediumMeteors; i++)
    {
        mediumMeteor.push_back(new Meteor(Vector2{ -100, -100 }, meteorImgUrl, meteorExplodeSfxUrl, Vector2{ 0,0 }, meteorsSpeed, GetRandomValue(0, 360), 20, false));
    }

    for (int i = 0; i < maxSmallMeteors; i++)
    {
        smallMeteor.push_back(new Meteor(Vector2{ -100, -100 }, meteorImgUrl, meteorExplodeSfxUrl, Vector2{ 0,0 }, meteorsSpeed, GetRandomValue(0, 360), 10, false));
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;

#pragma endregion

    HideCursor();
}

// Update game (one frame)
static void UpdateGame()
{
#pragma region Parallax Update

    scrollingBack -= 0.1f;
    scrollingMid -= 0.5f;

    if (scrollingBack <= -background.width * 2) scrollingBack = 0;
    if (scrollingMid <= -midground.width * 2) scrollingMid = 0;

#pragma endregion


    switch (gameState)
    {
    case GameState::MainMenu:

        if (!IsMusicPlaying(mainMusic)) 
        {
            StopMusicStream(gameplayMusic);
            PlayMusicStream(mainMusic);
        }
        else UpdateMusicStream(mainMusic);

        // Play button behaviour
        playButton->update();

        if (playButton->isClick())
        {
            InitGame();
            gameState = GameState::Gameplay;
        }

        tutorialButton->update();
        if (tutorialButton->isClick()) gameState = GameState::Tutorial;

        creditsButton->update();
        if (creditsButton->isClick()) gameState = GameState::Credits;

        quitButton->update();
        if (quitButton->isClick()) gameState = GameState::Quit;

        break;
    case GameState::Gameplay:

        if (!IsMusicPlaying(gameplayMusic)) 
        {
            StopMusicStream(mainMusic);
            PlayMusicStream(gameplayMusic); 
        }
        else UpdateMusicStream(gameplayMusic);

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

                hpPowerUp->update();

                if (hpPowerUp->getActive())
                {
                    if (CheckCollisionCircles(player->getPosition(), player->getRadius(), hpPowerUp->getPosition(), hpPowerUp->getRadius()))
                    {
                        hpPowerUp->setActive(false);
                        player->resetShield();
                    }
                }

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

                            SetSoundPitch(laserSfx, ((float)GetRandomValue(0, 45) / 100) + 1);
                            PlaySound(laserSfx);
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
                                bigMeteor[a]->explode();
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
                                mediumMeteor[b]->explode();
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
                                smallMeteor[c]->explode();
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

    case GameState::Tutorial:

        if (!IsMusicPlaying(mainMusic)) PlayMusicStream(mainMusic);
        else UpdateMusicStream(mainMusic);

        tutorialButton->update();
        if (tutorialButton->isClick()) gameState = GameState::MainMenu;

        break;

    case GameState::Credits:

        if (!IsMusicPlaying(mainMusic)) PlayMusicStream(mainMusic);
        else UpdateMusicStream(mainMusic);

        creditsButton->update();
        if (creditsButton->isClick()) gameState = GameState::MainMenu;

        break;
    }
}

// Draw game (one frame)
static void DrawGame()
{
    BeginDrawing();

    ClearBackground(BLACK);

#pragma region Parallax Draw

    DrawTextureEx(background, { scrollingBack, 20 }, 0.0f, 4.0f, WHITE);
    DrawTextureEx(background, { background.width * 2 + scrollingBack, 20 }, 0.0f, 4.0f, WHITE);

    DrawTextureEx(midground, { scrollingMid, 20 }, 0.0f, 4.0f, WHITE);
    DrawTextureEx(midground, { midground.width * 2 + scrollingMid, 20 }, 0.0f, 4.0f, WHITE);

#pragma endregion


    switch (gameState)
    {
    case GameState::MainMenu:

        DrawText("Asteroid xD", (float)GetScreenWidth() / 2 - MeasureText("Asteroid xD", 80) / 2, (float)GetScreenHeight() * 0.15f, 80, WHITE);
        DrawText("Created by Matias Galarza", GetScreenWidth() * 0.01f, GetScreenHeight() - 15, 15, GRAY);

        playButton->setText(TextFormat("JUGAR NIVEL %0i", level));
        playButton->draw();

        tutorialButton->setPivot({ 0.5f, 0.5f });
        tutorialButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.625f });
        tutorialButton->setText("TUTORIAL");
        tutorialButton->draw();

        creditsButton->setPivot({ 0.5f,0.5f });
        creditsButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.75f });
        creditsButton->setText("CREDITOS");
        creditsButton->draw();

        quitButton->draw();

        break;

    case GameState::Gameplay:
        if (!gameOver)
        {
            player->draw();

            hpPowerUp->draw();

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

    case GameState::Tutorial:

        DrawText("TUTORIAL", GetScreenWidth() / 2 - MeasureText("TUTORIAL", 40) / 2, GetScreenHeight() * 0.05f, 40, WHITE);
        DrawText(tutorialText, GetScreenWidth() / 2 - MeasureText(tutorialText, 15) / 2, GetScreenHeight() * 0.1f, 15, WHITE);

        tutorialButton->setPivot({ 0.5f, 1.0f });
        tutorialButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.98f});
        tutorialButton->setText("VOLVER");
        tutorialButton->draw();

        break;

    case GameState::Credits:

        DrawText("CREDITOS", GetScreenWidth() / 2 - MeasureText("CREDITOS", 40) / 2, GetScreenHeight() * 0.05f, 40, WHITE);
        DrawText(creditsText, GetScreenWidth() / 2 - MeasureText(creditsText, 14) / 2, GetScreenHeight() * 0.1f, 14, WHITE);

        creditsButton->setPivot({ 0.5f,1.0f });
        creditsButton->setPosition(Vector2{ (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.98f });
        creditsButton->setText("VOLVER");
        creditsButton->draw();
        break;
    }

    //Mouse position
#pragma region Mouse Crosshair Draw
    
    crosshair1Rot += GetFrameTime() * 90;
    crosshair2Rot -= GetFrameTime() * 45;

    // Crosshair 1
    DrawTexturePro(
        crosshair1,
        Rectangle{ 0,0,(float)crosshair1.width,(float)crosshair1.height },
        Rectangle{ (float)GetMouseX(), (float)GetMouseY(), (float)crosshair1.width * 0.5f,(float)crosshair1.height * 0.5f },
        Vector2{ ((float)crosshair1.width * 0.5f) / 2, ((float)crosshair1.height * 0.5f) / 2 },
        crosshair1Rot,
        GREEN);

    // Crosshair 2
    DrawTexturePro(
        crosshair2,
        Rectangle{ 0,0,(float)crosshair2.width,(float)crosshair2.height },
        Rectangle{ (float)GetMouseX(), (float)GetMouseY(), (float)crosshair2.width * 0.3f,(float)crosshair2.height * 0.3f },
        Vector2{ ((float)crosshair2.width * 0.3f) / 2, ((float)crosshair2.height * 0.3f) / 2 },
        crosshair2Rot,
        LIME);
    
#if _DEBUG
    DrawCircle(GetMouseX(), GetMouseY(), 5, RED);
#endif

#pragma endregion


    EndDrawing();
}

// Update and Draw (one frame)
static void UpdateDrawGameFrame()
{
    UpdateGame();
    DrawGame();
}

// Unload game variables
static void UnloadGame()
{
    // Delete Player
    delete player;

    delete hpPowerUp;

    // Delete Shoots
    for (Shoot* s : shoot)
    {
        delete s;
    }
    shoot.clear();

    // Delete Meteors
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

    // Delete UI
    delete playButton;
    delete tutorialButton;
    delete creditsButton;
    delete quitButton;
    delete shieldBar;
    delete reTryButton;
    delete returnMenuButton;

    UnloadImage(gameIcon);

    UnloadMusicStream(mainMusic);
    UnloadMusicStream(gameplayMusic);
    UnloadSound(laserSfx);
    CloseAudioDevice();
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
    while (!WindowShouldClose() && gameState != GameState::Quit)    // Detect window close button or ESC key
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
