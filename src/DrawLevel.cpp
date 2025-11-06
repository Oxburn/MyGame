#include "raylib.h"
#include "raymath.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>
#include <cmath>


std::vector<std::vector<Vector2>> curveParallel (const std::vector<Vector2>& points, float distance)
{
    std::vector<std::vector<Vector2>> parallelCurves;

    for (size_t i = 0; i < points.size() - 1; i++)
    {
        Vector2 A = points[i];
        Vector2 B = points[(i + 1) % points.size()];

        // Direction du segment
        Vector2 AB = Vector2Subtract(B, A);

        // Longueur
        float len = Vector2Length(AB);

        // Normale unitaire (perpendiculaire)
        Vector2 n = { -(AB.y) / len, AB.x / len };

        // Décalage selon la distance
        Vector2 offset = Vector2Scale(n, distance);

        // Nouveau segment [CD]
        Vector2 C = Vector2Add(A, offset);
        Vector2 D = Vector2Add(B, offset);

        // Raccourcissement de chaque extrémité
        if (len > distance)
        {
            Vector2 dir = Vector2Scale(AB, 1.0f / len);
            C = Vector2Add(C, Vector2Scale(dir, distance));
            D = Vector2Subtract(D, Vector2Scale(dir, distance));
            parallelCurves.push_back({C, D});
            // DrawLineEx(C, D, 5.0f, RED); // Affichage des courbes hors des surfaces (pour debug)
        }
    }

    return parallelCurves;
}

bool SegmentsIntersect(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4)
{
    float denom = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
    if (fabs(denom) < 1e-6f) return false; // segments parallèles ou colinéaires

    float ua = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / denom;
    float ub = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / denom;

    return (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1);
}

bool PointInTriangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c)
{
    float denom = (b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y);
    float w1 = ((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y)) / denom;
    float w2 = ((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y)) / denom;
    float w3 = 1 - w1 - w2;

    return (w1 >= 0 && w2 >= 0 && w3 >= 0);
}

bool SegmentTouchesTriangle(Vector2 A, Vector2 B, Vector2 C, Vector2 D, Vector2 E)
{
    // Si le segment touche un bord du triangle
    if (SegmentsIntersect(D, E, A, B)) return true;
    if (SegmentsIntersect(D, E, B, C)) return true;
    if (SegmentsIntersect(D, E, C, A)) return true;

    // Si un des points du segment est à l'intérieur du triangle
    if (PointInTriangle(D, A, B, C)) return true;
    if (PointInTriangle(E, A, B, C)) return true;

    return false; // Aucun contact
}

void DrawSurface(const std::vector<Vector2>& points, Color surfaceColor)
{
    std::vector<std::vector<Vector2>> parallelCurves = curveParallel(points, 1.0f);

    for (size_t i = 0; i < points.size() - 1; i++)
    {
        Vector2 A = points[i];
        Vector2 B = points[(i+1) % points.size()];

        for (size_t j = 0; j < points.size() - 1; j++)
        {
            if (i != j)
            {
                Vector2 C = points[j];
                bool contact = false;
                for (const auto& parallelCurve: parallelCurves)
                {
                    Vector2 D = parallelCurve[0];
                    Vector2 E = parallelCurve[1];

                    if (SegmentTouchesTriangle(A, B, C, D, E))
                    {
                        contact = true;
                        break;
                    }
                }

                if (!contact)
                {
                     DrawTriangle(A, B, C, surfaceColor);
                }
            }
        }
    }
}


void DrawLevel(const std::vector<Vector2>& points, float thickness, Color boundaryColor, Color surfaceColor, RenderDims dims)
{
    if (points.size() < 2) return;

    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----
    
    int renderTextureWidth = dims.renderTextureWidth;
    int renderTextureHeight = dims.renderTextureHeight;

    std::vector<Vector2> scaledPoints;
    scaledPoints.reserve(points.size());
    for (auto& p : points)
    {
        scaledPoints.push_back({
            (float)(p.x * renderTextureWidth / 100.0f),
            (float)(p.y * renderTextureHeight / 100.0f)
        });
    }


    // ---- Création des polydroites ----

    for (size_t i = 0; i < scaledPoints.size() - 1; i++)
    {
        Vector2 startPt = scaledPoints[i];
        Vector2 endtPt = scaledPoints[i+1];
        DrawLineEx(startPt, endtPt, thickness, boundaryColor);
    }


    // ---- Création des surfaces ----

    DrawSurface(scaledPoints, surfaceColor);
}