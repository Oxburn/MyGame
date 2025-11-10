#include "raylib.h"
#include "raymath.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>
#include <cmath>


std::vector<std::vector<Vector2>> CurveParallel (const std::vector<Vector2>& boundaryPoints, float distance)
{
    std::vector<std::vector<Vector2>> parallelCurves;

    for (size_t i = 0; i < boundaryPoints.size() - 1; i++)
    {
        Vector2 A = boundaryPoints[i];
        Vector2 B = boundaryPoints[(i + 1) % boundaryPoints.size()];

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

std::vector<std::vector<Vector2>> SurfaceInner(const std::vector<Vector2>& boundaryPoints, std::vector<std::vector<Vector2>>& parallelCurve)
{
    std::vector<std::vector<Vector2>> innerSurface;

    for (size_t i = 0; i < boundaryPoints.size() - 1; i++)
    {
        Vector2 A = boundaryPoints[i];
        Vector2 B = boundaryPoints[(i+1) % boundaryPoints.size()];

        for (size_t j = 0; j < boundaryPoints.size() - 1; j++)
        {
            if (i != j)
            {
                Vector2 C = boundaryPoints[j];
                bool contact = false;
                for (const auto& parallelPoints: parallelCurve)
                {
                    Vector2 D = parallelPoints[0];
                    Vector2 E = parallelPoints[1];
                    if (SegmentTouchesTriangle(A, B, C, D, E))
                    {
                        contact = true;
                        break;
                    }
                }

                if (!contact)
                {
                    innerSurface.push_back({A, B, C});
                }
            }
        }
    }

    return innerSurface;
}


void DrawLevel(LEVEL_DEFINITION levelDefinition)
{
    std::vector<Vector2> boundaryPoints = levelDefinition.boundaryPoints;
    float boundaryThickness = levelDefinition.boundaryThickness;
    Color boundaryColor = levelDefinition.boundaryColor;
    Color surfaceColor = levelDefinition.surfaceColor;
    std::vector<std::vector<Vector2>> parallelCurve = levelDefinition.parallelCurve;
    std::vector<std::vector<Vector2>> innerSurface = levelDefinition.innerSurface;

    if (boundaryPoints.size() < 2) return;


    // ---- Création des contours ----

    for (size_t i = 0; i < boundaryPoints.size() - 1; i++)
    {
        Vector2 A = {
            (float)(boundaryPoints[i].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(boundaryPoints[i].y * GLOBAL_RENDER_HEIGHT / 100),
        };
        Vector2 B = {
            (float)(boundaryPoints[i + 1].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(boundaryPoints[i + 1].y * GLOBAL_RENDER_HEIGHT / 100),
        };

        DrawLineEx(A, B, boundaryThickness, boundaryColor);
    }


    // ---- Création des parallèles (debug?) ----

    for (const auto& parallelLine : parallelCurve)
    {
        Vector2 A = {
            (float)(parallelLine[0].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(parallelLine[0].y * GLOBAL_RENDER_HEIGHT / 100)
        };
        Vector2 B = {
            (float)(parallelLine[1].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(parallelLine[1].y * GLOBAL_RENDER_HEIGHT / 100)
        };

        DrawLineEx(A, B, 1.0f, RED);
    }

    // ---- Création des surfaces ----

    for (const auto& innerTriangle : innerSurface)
    {
        Vector2 A = {
            (float)(innerTriangle[0].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(innerTriangle[0].y * GLOBAL_RENDER_HEIGHT / 100)
        };
        Vector2 B = {
            (float)(innerTriangle[1].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(innerTriangle[1].y * GLOBAL_RENDER_HEIGHT / 100)
        };
        Vector2 C = {
            (float)(innerTriangle[2].x * GLOBAL_RENDER_WIDTH / 100),
            (float)(innerTriangle[2].y * GLOBAL_RENDER_HEIGHT / 100)
        };

        DrawTriangle(A, B, C, surfaceColor);
    }
    //DrawSurface(boundaryPoints, parallelCurve, surfaceColor);
}