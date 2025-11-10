#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>
#include <string>
#include <cmath>


void StartScreen() {

    // ---- Liste des textes du menu ----

    struct TranslatedText {
        std::string key;
        std::vector<std::string> texts;
    };

    std::vector<TranslatedText> startScreenTexts = {
        {"Start", { "Commencer", "Start", "Comenzar" }},
        {"Options", { "Options", "Options", "Opciones" }},
        {"Quit", { "Quitter", "Quit", "Salir" }}
    };


    // ---- Gestion du curseur sélectionné (haut/bas) ----

    if (IsKeyPressed(KEY_DOWN)) GLOBAL_MENU_CURSOR_POS += 1;
    if (IsKeyPressed(KEY_UP))   GLOBAL_MENU_CURSOR_POS -= 1;
    if (GLOBAL_MENU_CURSOR_POS < 0) GLOBAL_MENU_CURSOR_POS = startScreenTexts.size() - 1;
    if (GLOBAL_MENU_CURSOR_POS >= (int)startScreenTexts.size()) GLOBAL_MENU_CURSOR_POS = 0;

    float time = GetTime();
    float cursorOffsetPx = sinf(time * 5.0f) * 5.0f;
    float cursorWidth = MeasureTextEx(GLOBAL_FONT, CURSOR, GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING).x;

    
    // ---- Accès aux autres écrans ----

    if (IsKeyPressed(KEY_ENTER)) {
        if (startScreenTexts[GLOBAL_MENU_CURSOR_POS].key == "Start") {
            GLOBAL_CURRENT_SCREEN = SCREEN_LVL1_1;
        } else if (startScreenTexts[GLOBAL_MENU_CURSOR_POS].key == "Options") {
            GLOBAL_CURRENT_SCREEN = SCREEN_OPTIONS;
        } else {
            UnloadRenderTexture(GLOBAL_RENDER_TEXTURE);
            UnloadFont(GLOBAL_FONT);
            CloseWindow();
        }
    }
    

    // ---- Dessin dans la RenderTexture ----

    BeginTextureMode(GLOBAL_RENDER_TEXTURE);

        ClearBackground(BACKGROUND_COLOR);

        for (int i = 0; i < (int)startScreenTexts.size(); i++) {

            std::string text = startScreenTexts[i].texts[GLOBAL_LANGUAGE_CURSOR_POS];
            float textWidth = MeasureTextEx(GLOBAL_FONT, text.c_str(), GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING).x;
            float posXRT = (GLOBAL_RENDER_WIDTH - textWidth) / 2.0f;
            float posYRT = GLOBAL_RENDER_HEIGHT / 2.0f + i * LINE_HEIGHT_FACTOR * GLOBAL_FONT_SIZE;
            float cursorOffsetRT = cursorOffsetPx * GLOBAL_SCALE_X;
            Vector2 position = { posXRT, posYRT };

            if (i == GLOBAL_MENU_CURSOR_POS) {
                Vector2 cursorPosition = position;
                cursorPosition.x += cursorOffsetRT - 2 * cursorWidth;
                DrawTextEx(GLOBAL_FONT, CURSOR, cursorPosition,
                           GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, SELECTED_TEXT_COLOR);
                DrawTextEx(GLOBAL_FONT, text.c_str(), position,
                           GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, SELECTED_TEXT_COLOR);
            } else {
                DrawTextEx(GLOBAL_FONT, text.c_str(), position,
                           GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, OTHER_TEXT_COLOR);
            }
        }

    EndTextureMode();
    DrawRenderTextureToScreen();
}
