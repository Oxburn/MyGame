#ifndef VARIABLES_H
#define VARIABLES_H

#include "raylib.h"
#include <string>
#include <vector>


////////////////////////////////////////////
// ---- VARIABLES GLOBALES COURANTES ---- //
////////////////////////////////////////////

// Rendu
extern RenderTexture2D GLOBAL_RENDER_TEXTURE;
extern int GLOBAL_RENDER_WIDTH;
extern int GLOBAL_RENDER_HEIGHT;
extern float GLOBAL_SCALE_X;

// Texte
extern int GLOBAL_FONT_SIZE;
extern int GLOBAL_TEXT_SPACING;
extern Font GLOBAL_FONT;

// Menu
extern bool GLOBAL_PAUSE;
extern float GLOBAL_DELAY;
extern int GLOBAL_MENU_CURSOR_POS;
extern int GLOBAL_OPTIONS_CURSOR_POS;
extern int GLOBAL_LANGUAGE_CURSOR_POS;
extern int GLOBAL_DIFFICULTY_CURSOR_POS;
extern int GLOBAL_STYLE_CURSOR_POS;
extern int GLOBAL_BRIGHTNESS_CURSOR_POS;
extern int GLOBAL_SOUND_CURSOR_POS;
extern int GLOBAL_MUSIC_CURSOR_POS;
//extern int GLOBAL_PAUSE_CURSOR_POS;

// Ecran
extern Vector2 GLOBAL_SCREEN_POS;

// Héros
extern float GLOBAL_HERO_RADIUS;
extern Vector2 GLOBAL_HERO_POS;
extern Vector2 GLOBAL_HERO_MOVE;
extern Vector2 GLOBAL_HERO_VELOCITY;
extern float GLOBAL_HERO_ROTATION;

// Physique colle
extern float GLOBAL_STICK_DURATION;
extern float GLOBAL_STICK_INTENSITY;         
extern float GLOBAL_START_STICK;
extern float GLOBAL_LAST_UNSTICK_TIME;
extern bool GLOBAL_WAS_STICKING;

// Clic saut
extern double GLOBAL_JUMP_CLICK_START_TIME;
extern bool GLOBAL_JUMP_IS_PRESSED;
extern bool GLOBAL_JUMP_IS_RELEASED;
extern double GLOBAL_JUMP_MAX_TIME;


////////////////////////////////////////
// ---- VARIABLES GLOBALES FIXES ---- //
////////////////////////////////////////

// Qualité
constexpr float WINDOW_BASE_WIDTH_RATIO = /*0.5*/ 0.5f;
constexpr float WINDOW_RATIO = 16.0f / 9.0f;
constexpr const  char* WINDOW_NAME = "MyGame";
constexpr int WINDOW_FRAMERATE = 60;
constexpr int TEXTURE_WIDTH = 1920; // HD : 1280 - 2K : 1920 - 4K : 3840

// Couleurs de base
Color TEXTURE_PRO_COLOR();
constexpr Color NO_DRAW_COLOR = BLACK;
constexpr Color BACKGROUND_COLOR = {30, 30, 30, 255};
constexpr Color INACTIVE_COLOR = {128, 128, 128, 255};
constexpr Color MOSS_COLOR = {151, 162, 70, 255};
constexpr Color GRASS_COLOR = {91, 147, 4, 255};
constexpr Color DIRT_COLOR = {95, 69, 48, 255};
constexpr Color ALUMINIUM_COLOR = {192, 192, 192, 255};

// Epaisseurs
constexpr float HERO_THICKNESS = 4.0f;
constexpr float MOSS_THICKNESS = 4.0f;
constexpr float GRASS_THICKNESS = 2.0f;
constexpr float ALUMINIUM_THICKNESS = 20.0f;

// Texte
constexpr const char* CURSOR = u8"➤";
constexpr const  char* FONT_FILE = "assets/DejaVuSansMono.ttf";
constexpr int CODEPOINT_COUNT = 224;
const Color SELECTED_TEXT_COLOR = {255, 255, 0, 255};
const Color OTHER_TEXT_COLOR = {211, 211, 211, 255};
const float LINE_HEIGHT_FACTOR = 1.3;
const float FONT_SIZE_RATIO = (float)(TEXTURE_WIDTH / 45000.0f);

// Menu
const float ADJUST_BAR_COOLDOWN = 0.05f;
const float ADJUST_TEXT_COOLDOWN = 0.3f;
const float OPTION_BAR_WIDTH = 300.0f;

// Colle
const float GLOBAL_STICK_COOLDOWN = 2.5f;

// Ecran
const float GLOBAL_BORDER_BEYOND = 10.0f;


//////////////////////////
// ---- STRUCTURES ---- //
//////////////////////////

// Ecrans
enum SCREENS {
    SCREEN_START,
    SCREEN_OPTIONS,
    SCREEN_LVL1_1,
    SCREEN_LVL1_2
};
extern SCREENS GLOBAL_CURRENT_SCREEN;

// Définition des morceaux de niveaux
struct LEVEL_PART_DEFINITION {
    std::vector<Vector2> boundaryPoints;
    float boundaryThickness;
    Color boundaryColor;
    Color surfaceColor;
};


// Définition des niveaux
struct LEVEL_DEFINITION {
    std::vector<Vector2> boundaryPoints;
    float boundaryThickness;
    Color boundaryColor;
    Color surfaceColor;
    std::vector<std::vector<Vector2>> parallelCurve;
    std::vector<std::vector<Vector2>> innerSurface;
};

#endif
