#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>


void DrawPolyline(const std::vector<Vector2>& points, float thickness, Color color, RenderDims dims)
{
    if (points.size() < 2) return;

    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----
    
    int renderTextureWidth = dims.renderTextureWidth;
    int renderTextureHeight = dims.renderTextureHeight;


    // ---- Création de la polydroite ----
    for (size_t i = 0; i < points.size() - 1; i++)
    {
        Vector2 startPt = { 
            (float)(points[i].x * renderTextureWidth/100),
            (float)(points[i].y * renderTextureHeight/100)
        };
        Vector2 endtPt = {
            (float)(points[i+1].x * renderTextureWidth/100),
            (float)(points[i+1].y * renderTextureHeight/100)
        };

        DrawLineEx(startPt, endtPt, thickness, color);
    }
}