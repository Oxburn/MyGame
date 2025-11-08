#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "DrawLevel.h"
#include "DrawHero.h"
#include <vector>


void Level1_Screen2() {

    // ---- Points du niveau ----

    std::vector<Vector2> moss1Pts = {
        {15, 0},
        {5, 35},
        {5, 75},
        {30, 75},
        {30, 80},
        {55, 80},
        {55, 85},
        {95, 85},
        {95, 80},
        {100, 80}
    };

    std::vector<Vector2> moss2Pts = {
        {100, 60},
        {85, 60},
        {85, 50},
        {80, 0}
    };

    std::vector<Vector2> moss3Pts = {
        {50, 50},
        {60, 50},
        {60, 45},
        {50, 45},
        {50, 50}
    };

    std::vector<Vector2> moss4Pts = {
        {30, 65},
        {40, 65},
        {40, 60},
        {30, 60},
        {30, 65}
    };

    std::vector<Vector2> grassPts = {
        {98, 59},
        {98, 81}
    };


    // Regroupement des listes

    std::vector<LEVEL_DEFINITION> levelDefinitions = {
        { moss1Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR },
        { grassPts, GRASS_THICKNESS, GRASS_COLOR, DIRT_COLOR },
        { moss2Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR },
        { moss3Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR },
        { moss4Pts, MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR }
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