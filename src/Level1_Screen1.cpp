#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "DrawLevel.h"
#include "DrawHero.h"
#include <vector>

float b = 10; // Dépassement des bords

void Level1_Screen1() {

    // ---- Points du niveau ----

    LEVEL_PART_DEFINITION mossWall1 = {
        {
            {-b, -b},
            {-b, 200 + b},
            {40, 200 + b},
            {40, 90},
            {10, 90},
            {10, 70},
            {45, 70},
            {45, -b},
            {-b, -b}
        },
        MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR
    };

    LEVEL_PART_DEFINITION mossWall2 = {
        {
            {100 + b, -b},
            {60, -b},
            {60, 170},
            {90, 200 + b},
            {100 + b, 200 + b},
            {100 + b, b}
        },
        MOSS_THICKNESS, MOSS_COLOR, DIRT_COLOR
    };

    LEVEL_PART_DEFINITION mossPlatform = {
        {
            {50, 60},
            {50, 62},
            {55, 52},
            {55, 50},
            {50, 60}
        },
        MOSS_THICKNESS, MOSS_COLOR, MOSS_COLOR
    };

    LEVEL_PART_DEFINITION aluminiumDoor = {
        {
            {43, 0},
            {62, 0}
        },
        ALUMINIUM_THICKNESS, ALUMINIUM_COLOR, ALUMINIUM_COLOR
    };
    
    std::vector<LEVEL_PART_DEFINITION> levelPartDefinitions = {
        mossWall1,
        mossWall2,
        mossPlatform,
        aluminiumDoor
    };


    // ---- Ajout des courbes parallèles ----

    std::vector<LEVEL_DEFINITION> levelDefinitions;

    for (const auto& levelPartDefinition : levelPartDefinitions)
    {
        std::vector<Vector2> boundaryPoints = levelPartDefinition.boundaryPoints;
        float boundaryThickness = levelPartDefinition.boundaryThickness;
        Color boundaryColor = levelPartDefinition.boundaryColor;
        Color surfaceColor = levelPartDefinition.surfaceColor;

        std::vector<std::vector<Vector2>> parallelCurve = CurveParallel(boundaryPoints, 0.1f);
        std::vector<std::vector<Vector2>> innerSurface = SurfaceInner(boundaryPoints, parallelCurve);

        levelDefinitions.push_back({
            boundaryPoints,
            boundaryThickness,
            boundaryColor,
            surfaceColor,
            parallelCurve,
            innerSurface
        });

    }


    // ---- Dessin dans la RenderTexture ----

    BeginTextureMode(GLOBAL_RENDER_TEXTURE);
        ClearBackground(BACKGROUND_COLOR);
        DrawHero(levelDefinitions);
        for (const auto& levelDefinition : levelDefinitions) {
            DrawLevel(levelDefinition);
        }
    EndTextureMode();

    DrawRenderTextureToScreen();
}