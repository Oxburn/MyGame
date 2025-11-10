#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "StartScreen.h"
#include <vector>
#include <string>
#include <cmath>


void OptionsScreen() {

    // ---- Listes des textes des options ----

    struct TranslatedText {
        std::string key;
        std::vector<std::string> texts;
    };

    std::vector<TranslatedText> optionsScreenTexts = {
        {"Language", { "Langue", "Language", "Idioma" }},
        {"Difficulty", { "Difficulté", "Difficulty", "Dificultad" }},
        {"Style", { "Design", "Style", "Estilo" }},
        {"Brightness", { "Luminosité", "Brightness", "Brillo" }},
        {"Sound", { "Son", "Sound", "Sonido" }},
        {"Music", { "Musique", "Music", "Música" }},
        {"Back", { "Retour", "Back", "Vuelto" }}
    };

    std::vector<std::pair<std::string, std::string>> languageTexts = {
        {"French", "Français"},
        {"English", "English"},
        {"Spanish", "Español"}
    };

    std::vector<TranslatedText> difficultyTexts = {
        {"Easy", { "Facile", "Easy", "Fácil" }},
        {"Normal", { "Normal", "Normal", "Normal" }},
        {"Hard", { "Difficile", "Hard", "Difícil" }}
    };

    std::vector<TranslatedText> styleTexts = {
        {"Original", { "Original", "Original", "Original" }},
        {"Wired", { "Filaire", "Wired", "Cableado" }}
    };


    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----

    //int scaleX = dims.scaleX;

    float maxTextWidth = 0;
    for (int i = 0; i < (int)optionsScreenTexts.size(); i++) {
        std::string text = optionsScreenTexts[i].texts[GLOBAL_LANGUAGE_CURSOR_POS];
        float textWidth = MeasureTextEx(GLOBAL_FONT, text.c_str(), GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING).x;
        maxTextWidth = std::max(maxTextWidth, textWidth);
    }
    

    // ---- Gestion du curseur sélectionné (haut/bas) ----

    if (IsKeyPressed(KEY_DOWN)) GLOBAL_OPTIONS_CURSOR_POS += 1;
    if (IsKeyPressed(KEY_UP))   GLOBAL_OPTIONS_CURSOR_POS -= 1;
    if (GLOBAL_OPTIONS_CURSOR_POS < 0) GLOBAL_OPTIONS_CURSOR_POS = optionsScreenTexts.size() - 1;
    if (GLOBAL_OPTIONS_CURSOR_POS >= (int)optionsScreenTexts.size()) GLOBAL_OPTIONS_CURSOR_POS = 0;

    float dt = GetFrameTime();
    GLOBAL_DELAY = std::max(0.0f, GLOBAL_DELAY - dt);

    if (GLOBAL_DELAY == 0.0f) {
        const std::string &key = optionsScreenTexts[GLOBAL_OPTIONS_CURSOR_POS].key;

        // Helper : options cycliques (Language / Difficulty / Style)
        auto adjustCyclingIndex = [&](int &cursor, int size) {
            if (IsKeyDown(KEY_RIGHT)) {
                cursor = (cursor + 1) % size;
                GLOBAL_DELAY = ADJUST_TEXT_COOLDOWN;
            }
            else if (IsKeyDown(KEY_LEFT)) {
                cursor = (cursor - 1 + size) % size;
                GLOBAL_DELAY = ADJUST_TEXT_COOLDOWN;
            }
        };

        // Helper : options de 0 à 100 (Brightness / Sound / Music)
        auto adjustRangedIndex = [&](int &value) {
            if (IsKeyDown(KEY_RIGHT) && value < 100) {
                ++value;
                GLOBAL_DELAY = ADJUST_BAR_COOLDOWN;
            }
            else if (IsKeyDown(KEY_LEFT) && value > 0) {
                --value;
                GLOBAL_DELAY = ADJUST_BAR_COOLDOWN;
            }
        };

        if (key == "Language") {
            adjustCyclingIndex(GLOBAL_LANGUAGE_CURSOR_POS, languageTexts.size());
        }
        else if (key == "Difficulty") {
            adjustCyclingIndex(GLOBAL_DIFFICULTY_CURSOR_POS, difficultyTexts.size());
        }
        else if (key == "Style") {
            adjustCyclingIndex(GLOBAL_STYLE_CURSOR_POS, styleTexts.size());
        }
        else if (key == "Brightness") {
            adjustRangedIndex(GLOBAL_BRIGHTNESS_CURSOR_POS);
        }
        else if (key == "Sound") {
            adjustRangedIndex(GLOBAL_SOUND_CURSOR_POS);
        }
        else if (key == "Music") {
            adjustRangedIndex(GLOBAL_MUSIC_CURSOR_POS);
        }
    }
    
    float time = GetTime();
    float cursorOffsetPx = sinf(time * 5.0f) * 5.0f;
    float cursorWidth = MeasureTextEx(GLOBAL_FONT, CURSOR, GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING).x;


    // ---- Accès aux autres écrans ----

    if (IsKeyPressed(KEY_ENTER)) {
        if (optionsScreenTexts[GLOBAL_OPTIONS_CURSOR_POS].key == "Back") {
            GLOBAL_CURRENT_SCREEN = SCREEN_START;
        }
    }


    // ---- Dessin dans la RenderTexture ----

    BeginTextureMode(GLOBAL_RENDER_TEXTURE);

        ClearBackground(BACKGROUND_COLOR);

        for (int i = 0; i < (int)optionsScreenTexts.size(); i++) {

            std::string text = optionsScreenTexts[i].texts[GLOBAL_LANGUAGE_CURSOR_POS];

            float posXRT = GLOBAL_RENDER_WIDTH / 10.0f;
            float posYRT = GLOBAL_RENDER_HEIGHT / 10.0f + i * LINE_HEIGHT_FACTOR * GLOBAL_FONT_SIZE;
            float cursorOffsetRT = cursorOffsetPx * GLOBAL_SCALE_X;
            Vector2 textPosition = { posXRT, posYRT };
            Vector2 barPosition = { posXRT + 1.2f * maxTextWidth, posYRT + GLOBAL_FONT_SIZE * 0.5f };
            Vector2 optionPosition = { posXRT + 1.2f * maxTextWidth, posYRT };
            Vector2 valuePosition = { posXRT + 1.2f * maxTextWidth + 1.1f * OPTION_BAR_WIDTH, posYRT};

            Color textColor;
            if (i == GLOBAL_OPTIONS_CURSOR_POS) {
                Vector2 cursorPosition = textPosition;
                cursorPosition.x += cursorOffsetRT - 2 * cursorWidth;
                textColor = SELECTED_TEXT_COLOR;
                DrawTextEx(GLOBAL_FONT, CURSOR, cursorPosition,
                           GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, textColor);
            } else {
                textColor = OTHER_TEXT_COLOR;
            }

            DrawTextEx(GLOBAL_FONT, text.c_str(), textPosition,
                    GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, textColor);

            const std::string &key = optionsScreenTexts[i].key;

            // Cas 1 : options avec libellé simple (texte)
            auto drawTextOption = [&](const std::string &value) {
                DrawTextEx(GLOBAL_FONT, value.c_str(), optionPosition,
                        GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, textColor);
            };

            // Cas 2 : options avec barre + valeur numérique
            auto drawBarOption = [&](int value) {
                float barHeight   = GLOBAL_FONT_SIZE * 0.2f;
                float filledWidth = OPTION_BAR_WIDTH * (value / 100.0f);
                std::string optionValue = std::to_string(value);

                DrawRectangle(barPosition.x, barPosition.y, OPTION_BAR_WIDTH, barHeight, INACTIVE_COLOR);
                DrawRectangle(barPosition.x, barPosition.y, filledWidth, barHeight, textColor);

                DrawTextEx(GLOBAL_FONT, optionValue.c_str(), valuePosition,
                        GLOBAL_FONT_SIZE, GLOBAL_TEXT_SPACING, textColor);
            };

            // Sélection de l'affichage en fonction du type d'option
            if (key == "Language") {
                drawTextOption(languageTexts[GLOBAL_LANGUAGE_CURSOR_POS].second);
            }
            else if (key == "Difficulty") {
                drawTextOption(difficultyTexts[GLOBAL_DIFFICULTY_CURSOR_POS].texts[GLOBAL_LANGUAGE_CURSOR_POS]);
            }
            else if (key == "Style") {
                drawTextOption(styleTexts[GLOBAL_STYLE_CURSOR_POS].texts[GLOBAL_LANGUAGE_CURSOR_POS]);
            }
            else if (key == "Brightness" || key == "Sound" || key == "Music") {
                int value = (key == "Brightness") ? GLOBAL_BRIGHTNESS_CURSOR_POS
                        : (key == "Sound")      ? GLOBAL_SOUND_CURSOR_POS
                                                : GLOBAL_MUSIC_CURSOR_POS;
                drawBarOption(value);
            }
        }

    EndTextureMode();
    DrawRenderTextureToScreen();
}