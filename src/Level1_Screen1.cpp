#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "DrawLevel.h"
#include "DrawHero.h"
#include <vector>

float b = 10; // Dépassement des bords

void Level1_Screen1() {

    // ---- Points du niveau ----

    std::vector<Vector2> moss1Pts = {
        {-b, -b},
        {-b, 200 + b},
        {40, 200 + b},
        {40, 90},
        {10, 90},
        {10, 70},
        {45, 70},
        {45, -b},
        {-b, -b}
    };

    std::vector<Vector2> moss2Pts = {
        {100 + b, -b},
        {60, -b},
        {60, 170},
        {90, 200 + b},
        {100 + b, 200 + b},
        {100 + b, b}
    };

    /// A Supprimer lorsque physique héros OK ///
    std::vector<Vector2> tempMossPts = {
        {50, 60},
        {50, 62},
        {55, 52},
        {55, 50},
        {50, 60}
    };

    /// A Supprimer lorsque physique héros OK ///
    

    // Regroupement des listes

    std::vector<LEVEL_DEFINITION> levelDefinitions = {
        { moss1Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR },
        { moss2Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR },
        { tempMossPts, MOSS_THICKNESS, MOSS_COLOR, MOSS_COLOR }
    };

    
    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----

    RenderDims dims = ComputeRenderDims();
    

    // ---- Dessin dans la RenderTexture ----

    BeginTextureMode(GLOBAL_RENDER_TEXTURE);
        ClearBackground(BACKGROUND_COLOR);
        for (const auto& levelDefinition : levelDefinitions) {
            DrawLevel(levelDefinition, dims);
        }
        DrawHero(levelDefinitions, dims);
    EndTextureMode();

    DrawRenderTextureToScreen(dims);
}