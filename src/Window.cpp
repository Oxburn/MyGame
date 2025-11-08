#include "raylib.h"
#include "rlgl.h"
#include "Variables.h"
#include "DetectSystemLanguage.h"
#include "StartScreen.h"
#include "OptionsScreen.h"
#include "Level1_Screen1.h"
#include "Level1_Screen2.h"


void Window() {

    // ---- Initialisation de la fenêtre ----

    Image icon = LoadImage("assets/gameIcon.png");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(0, 0, WINDOW_NAME);
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetTargetFPS(WINDOW_FRAMERATE);
    SetExitKey(KEY_NULL);


    // ---- Calcul dimensions et positions initiales ----
    
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int windowWidth = (int)(screenWidth * WINDOW_BASE_WIDTH_RATIO);
    int windowHeight = (int)(windowWidth / WINDOW_RATIO);

    int windowPositionX = (screenWidth - windowWidth) / 2;
    int windowPositionY = (screenHeight - windowHeight) / 2;

    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition(windowPositionX, windowPositionY);


    // ---- Chargement de la texture ----

    int textureHeight = (int)(TEXTURE_WIDTH / WINDOW_RATIO);
    GLOBAL_RENDER_TEXTURE = LoadRenderTexture(TEXTURE_WIDTH, textureHeight);


    // ---- Chargement de la police ----

    // Debug si Open GL < 4
    int glVersion = rlGetVersion();
    int fontCoef = 1;
    if (glVersion < 4)
    {
        fontCoef = 3;
    }

    // Arguments de textes
    int codepoints[CODEPOINT_COUNT];
    for (int i = 0; i < CODEPOINT_COUNT; i++) codepoints[i] = 32 + i;
    codepoints[CODEPOINT_COUNT - 1] = 0x27A4; // ➤
    GLOBAL_FONT_SIZE = (int)(fontCoef * windowHeight * FONT_SIZE_RATIO);
    GLOBAL_TEXT_SPACING = (int)(GLOBAL_FONT_SIZE * 0.05);
    GLOBAL_FONT = LoadFontEx(FONT_FILE, GLOBAL_FONT_SIZE, codepoints, CODEPOINT_COUNT);
    

    // ---- Comportement dynamique de la fenêtre ----

    bool isFullScreen = false;
    bool GLOBAL_PAUSE = false;
    GLOBAL_DELAY = 0.0f;
    GLOBAL_MENU_CURSOR_POS = 0;
    GLOBAL_OPTIONS_CURSOR_POS = 0;
    GLOBAL_LANGUAGE_CURSOR_POS = DetectSystemLanguage();
    GLOBAL_DIFFICULTY_CURSOR_POS = 0;
    GLOBAL_STYLE_CURSOR_POS = 0;
    GLOBAL_BRIGHTNESS_CURSOR_POS = 100;
    GLOBAL_SOUND_CURSOR_POS = 100;
    GLOBAL_MUSIC_CURSOR_POS = 100;
    GLOBAL_CURRENT_SCREEN = SCREEN_START;
    GLOBAL_HERO_PREVIOUS_TIME = GetTime();
    GLOBAL_HERO_RADIUS = 2.0f;
    //GLOBAL_HERO_POS = {52, 0};
    GLOBAL_HERO_POS = {20.0f, 80.0f};
    GLOBAL_HERO_MOVE = {0.0f, 0.0f};
    GLOBAL_HERO_VELOCITY = {0.0f, 0.0f};
    GLOBAL_HERO_ROTATION = 0.0f;

    // Clic saut
    GLOBAL_JUMP_CLICK_START_TIME = 0.0;
    GLOBAL_JUMP_IS_PRESSED = false;
    GLOBAL_JUMP_IS_RELEASED = true;
    GLOBAL_JUMP_MAX_TIME = 0.5; // Durée max de saut en seconde

    while (!WindowShouldClose())
    {
        // Gestion du plein écran
        if (IsKeyPressed(KEY_F11) or IsKeyPressed(KEY_ESCAPE)) {
            if (!isFullScreen) {
                SetWindowSize(windowWidth, windowHeight);
                ToggleFullscreen();
                isFullScreen = true;
            } else {
                SetWindowSize(windowWidth, windowHeight);
                ToggleFullscreen();
                isFullScreen = false;
            }
        }
        
        // CHoix de l'écran
        if (GLOBAL_CURRENT_SCREEN == SCREEN_START) {
            StartScreen();
        }else if (GLOBAL_CURRENT_SCREEN == SCREEN_OPTIONS) {
            OptionsScreen();
        } else if (GLOBAL_CURRENT_SCREEN == SCREEN_LVL1_1) {
            Level1_Screen1();
        }  else if (GLOBAL_CURRENT_SCREEN == SCREEN_LVL1_2) {
            Level1_Screen2();
        } 
    }


    // ---- Déchargement ----

    UnloadRenderTexture(GLOBAL_RENDER_TEXTURE);
    UnloadFont(GLOBAL_FONT);
    CloseWindow();
}