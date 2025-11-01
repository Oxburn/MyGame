#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "DrawPolyline.h"
#include "DrawHero.h"
#include <vector>


void Level1_Screen1() {

    // ---- Points du niveau ----

    std::vector<Vector2> moss1Pts = {
        {45, 0},
        {45, 70},
        {10, 70},
        {10, 90},
        {40, 90},
        {40, 200}
    };

    std::vector<Vector2> moss2Pts = {
        {60, 0},
        {60, 170},
        {90, 200}
    };

    /// A Supprimer lorsque physique héros OK ///
    std::vector<Vector2> tempMossPts = {
        {50, 60},
        {55, 50},
        {55, 52},
        {50, 62},
        {50, 60}
    };
    /// A Supprimer lorsque physique héros OK ///
    

    // Regroupement des listes

    std::vector<POLYLINE> polylines = {
        { moss1Pts, MOSS_THICKNESS, MOSS_COLOR },
        { moss2Pts, MOSS_THICKNESS, MOSS_COLOR },
        { tempMossPts, MOSS_THICKNESS, MOSS_COLOR }
    };

    
    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----

    RenderDims dims = ComputeRenderDims();
    

    // ---- Dessin dans la RenderTexture ----

    BeginTextureMode(GLOBAL_RENDER_TEXTURE);

        ClearBackground(BACKGROUND_COLOR);
        for (const auto& poly : polylines) {
            DrawPolyline(poly.points, poly.thickness, poly.color, dims);
        }
        DrawHero(polylines, dims);

    EndTextureMode();
    DrawRenderTextureToScreen(dims);
}