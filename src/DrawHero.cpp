#include "raylib.h"
#include "raymath.h"
#include "Variables.h"
#include "RenderUtils.h"
#include "PointsHero.h"
#include <vector>
#include <cmath>
#include <algorithm> 

#include <iostream>


// Paramètres
float GLOBAL_MAX_SPEED = 3.0f;
float GLOBAL_HERO_ACCELERATION = 0.5f;
float GLOBAL_HERO_BOUNCE = 1.0f;      // Rebond du héros
Vector2 GLOBAL_GRAVITY_FORCE = {0.0f, 7.0f};
float GLOBAL_JUMP_POWER = 15.0f; // 8.0f (un peu plus que la gravité) -> Saut minimal

float IntersectSegmentsDistance(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4)
{
    // p1-p2 = segment du héros (rayon)
    // p3-p4 = segment de la polyligne
    float s1_x = p2.x - p1.x;
    float s1_y = p2.y - p1.y;
    float s2_x = p4.x - p3.x;
    float s2_y = p4.y - p3.y;

    float denom = (-s2_x * s1_y + s1_x * s2_y);
    if (fabsf(denom) < 1e-6f)
        return GLOBAL_HERO_RADIUS; // segments parallèles ou presque

    float s = (-s1_y * (p1.x - p3.x) + s1_x * (p1.y - p3.y)) / denom;
    float t = ( s2_x * (p1.y - p3.y) - s2_y * (p1.x - p3.x)) / denom;

    // Si les deux segments se coupent dans leurs bornes respectives
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Calcul du point d’intersection
        Vector2 intersection = { p1.x + (t * s1_x), p1.y + (t * s1_y) };

        // Retourne la distance entre le héros et ce point
        return Vector2Distance(p1, intersection);
    }

    return GLOBAL_HERO_RADIUS; // pas d’intersection
}

std::vector<Vector2> GetProximity(const std::vector<LEVEL_DEFINITION>& levelDefinitions)
{
    std::vector<Vector2> results;

    std::vector<std::pair<float, Vector2>> heroPoints = PointsHero();
    for (const auto& heroPoint : heroPoints)
    {
        Vector2 A = GLOBAL_HERO_POS;
        Vector2 B = heroPoint.second;
        float angle = heroPoint.first;

        // Longueur maximale possible
        float maxScaledLen = sqrtf((B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y));
        // fallback si dégénéré
        if (maxScaledLen <= 1e-6f) {
            continue;
        }
        // Distance en espace non-scalé (radial)
        float minDist = GLOBAL_HERO_RADIUS; 

        for (const auto& levelDefinition : levelDefinitions)
        {
            for (size_t j = 0; j + 1 < levelDefinition.boundaryPoints.size(); j++)
            {
                // dist est la distance le long du segment A->B (dans l'espace compressé)
                float dist = IntersectSegmentsDistance(
                    A, B,
                    levelDefinition.boundaryPoints[j],
                    levelDefinition.boundaryPoints[j + 1]);
                if (dist >= 0.0f)
                {
                    // convertir dist (espace compressé) -> t (0..1) -> distance réelle non-scalée
                    float t = dist / maxScaledLen;
                    if (t < 0.0f) continue;
                    if (t > 1.0f) continue;

                    float realDist = t * GLOBAL_HERO_RADIUS; // ramène dans la métrique d'origine
                    if (realDist < minDist)
                    {
                        minDist = realDist;
                    }
                }
            }
        }

        results.push_back({ angle, minDist });
    }

    return results;
}

Vector2 ComputeForces(std::vector<Vector2> proximityData, RenderDims dims)
{
    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----
    
    int renderTextureWidth = dims.renderTextureWidth;
    int renderTextureHeight = dims.renderTextureHeight;


    // ---- Pour rassembler les forces et faire le bilan ----

    std::vector<Vector2> forces = {GLOBAL_GRAVITY_FORCE};


    // ---- Forces de réaction ----

    for (size_t i = 0; i < proximityData.size(); i++)
    {
        float angle = proximityData[i].x;
        float distance = proximityData[i].y;

        if (distance < GLOBAL_HERO_RADIUS) {

            float penetration = GLOBAL_HERO_RADIUS - distance;
            float totalForce = std::min(GLOBAL_HERO_BOUNCE * penetration, GLOBAL_MAX_SPEED);

            float reactionForceX = totalForce * cos(angle + PI);
            float reactionForceY = totalForce * sin(angle + PI);
            Vector2 reactionForce = {reactionForceX, reactionForceY};
            forces.push_back(reactionForce);
        }
    }


    // ---- Forces du saut (proportionnelles aux Forces de réaction !) ----

    if (IsKeyPressed(KEY_S))
    {
        GLOBAL_JUMP_CLICK_START_TIME = GetTime();
        GLOBAL_JUMP_IS_PRESSED = true;
    }
    if (GLOBAL_JUMP_IS_PRESSED && IsKeyDown(KEY_S))
    {
        double duration = GetTime() - GLOBAL_JUMP_CLICK_START_TIME;
        duration = std::min(duration, GLOBAL_JUMP_MAX_TIME);

        float sumY = -1.0f;
        std::vector<Vector2> newForces;
        for (size_t i = 1; i < forces.size() - 1; i++)
        {
            Vector2 reactionForce = forces[i];
            newForces.push_back(reactionForce);
            sumY += reactionForce.y;
        }

        float jumpCoef = GLOBAL_JUMP_POWER * GLOBAL_JUMP_POWER / (std::pow(duration + 1.0f, 10.0f)) / -sumY;
        jumpCoef = std::min(jumpCoef, GLOBAL_JUMP_POWER);

        for (const auto& newForce: newForces)
        {
            Vector2 jumpForce =
            {
                (float)(jumpCoef * newForce.x),
                (float)(jumpCoef * newForce.y)
            };
            forces.push_back(jumpForce);
        }
    }
    

    // ---- Forces de déplacement ----

    if (IsKeyDown(KEY_RIGHT) && GLOBAL_HERO_MOVE.x < GLOBAL_MAX_SPEED)
    {
        GLOBAL_HERO_MOVE.x += GLOBAL_HERO_ACCELERATION;
    }
    if (IsKeyDown(KEY_LEFT) && GLOBAL_HERO_MOVE.x > -GLOBAL_MAX_SPEED)
    {
        GLOBAL_HERO_MOVE.x -= GLOBAL_HERO_ACCELERATION;
    }
    if (IsKeyUp(KEY_RIGHT) && IsKeyUp(KEY_LEFT))
    {
        GLOBAL_HERO_MOVE.x = 0.0f;
    }

    if (IsKeyDown(KEY_DOWN) && GLOBAL_HERO_MOVE.y < GLOBAL_MAX_SPEED)
    {
        GLOBAL_HERO_MOVE.y += GLOBAL_HERO_ACCELERATION;
    }
    if (IsKeyPressed(KEY_UP) && GLOBAL_HERO_MOVE.y > -GLOBAL_MAX_SPEED)
    {
        GLOBAL_HERO_MOVE.y -= GLOBAL_HERO_ACCELERATION;
    }
    if (IsKeyUp(KEY_UP) && IsKeyUp(KEY_DOWN))
    {
        GLOBAL_HERO_MOVE.y = 0.0f;
    }

    forces.push_back(GLOBAL_HERO_MOVE);


    // ---- Force de frottements ----



    // ---- Dessin des vecteurs de force (à supprimer à termes ?) ----

    for (size_t i = 0; i < forces.size(); i++)
    {
        Vector2 start = {
            (float)(GLOBAL_HERO_POS.x * renderTextureWidth / 100),
            (float)(GLOBAL_HERO_POS.y * renderTextureHeight / 100)
        };

        Vector2 end = {
            (float)((GLOBAL_HERO_POS.x + forces[i].x) * renderTextureWidth / 100),
            (float)((GLOBAL_HERO_POS.y + forces[i].y) * renderTextureHeight / 100)
        };

        // Corps de la flèche
        DrawLineEx(start, end, 1.0f, RED);

        // Direction et longueur
        Vector2 dir = Vector2Subtract(end, start);
        float length = Vector2Length(dir);
        if (length == 0) continue;

        // Normalisation
        Vector2 norm = Vector2Scale(dir, 1.0f / length);

        // Taille et angle de la tête
        float headLength = 10.0f;
        float headAngle = DEG2RAD * 25.0f;

        float cosA = cosf(headAngle);
        float sinA = sinf(headAngle);

        // Calcul des deux directions (rotation gauche et droite)
        Vector2 leftDir = {
            norm.x * cosA - norm.y * sinA,
            norm.x * sinA + norm.y * cosA
        };
        Vector2 rightDir = {
            norm.x * cosA + norm.y * sinA,
            -norm.x * sinA + norm.y * cosA
        };

        // Points finaux de la tête
        Vector2 left = {
            end.x - leftDir.x * headLength,
            end.y - leftDir.y * headLength
        };
        Vector2 right = {
            end.x - rightDir.x * headLength,
            end.y - rightDir.y * headLength
        };

        // Dessin de la tête
        DrawLineEx(end, left, 1.0f, RED);
        DrawLineEx(end, right, 1.0f, RED);
    }


    // ---- Bilan des forces ----

    float totalForceX = 0.0f;
    float totalForceY = 0.0f;
    for (const auto& force : forces)
    {
        totalForceX += force.x;
        totalForceY += force.y;
    }

    return {totalForceX, totalForceY};
}

void DrawHero(std::vector<LEVEL_DEFINITION>& levelDefinitions, RenderDims dims)
{
    // ---- Calcul des éléments pour le respect du ratio (fenêtre) ----
    
    int renderTextureWidth = dims.renderTextureWidth;
    int renderTextureHeight = dims.renderTextureHeight;


    // ---- Dessin du héros ----

    std::vector<Vector2> proximityData = GetProximity(levelDefinitions);

    for (size_t i = 0; i < proximityData.size(); i++)
    {
        float startAngle = proximityData[i].x;
        float startDistance = proximityData[i].y;
        
        float endAngle = proximityData[(i + 1) % proximityData.size()].x;
        float endDistance = proximityData[(i + 1) % proximityData.size()].y;

        float startPtX = GLOBAL_HERO_POS.x + startDistance * cosf(startAngle);
        float startPtY = GLOBAL_HERO_POS.y + startDistance * sinf(startAngle);
        float endPtX = GLOBAL_HERO_POS.x + endDistance * cosf(endAngle);
        float endPtY = GLOBAL_HERO_POS.y + endDistance * sinf(endAngle);

        startPtX = GLOBAL_HERO_POS.x + (startPtX - GLOBAL_HERO_POS.x) / WINDOW_RATIO;
        endPtX   = GLOBAL_HERO_POS.x + (endPtX - GLOBAL_HERO_POS.x) / WINDOW_RATIO;

        Vector2 startPt = { 
            (float)(startPtX * renderTextureWidth/100),
            (float)(startPtY * renderTextureHeight/100)
        };
        Vector2 endtPt = {
            (float)(endPtX * renderTextureWidth/100),
            (float)(endPtY * renderTextureHeight/100)
        };

        float t = (float)i / (float)(proximityData.size() - 1); // de 0.0 à 1.0
        unsigned char r = (unsigned char)(0 + t * 50);   // de 0 à 50
        unsigned char g = (unsigned char)(0 + t * 100);  // de 0 à 100
        unsigned char b = (unsigned char)(150 + t * 105); // de 150 à 255
        Color heroColor = {r, g, b, 255};

        DrawLineEx(startPt, endtPt, 2.0f, heroColor);
    }
    

    // ---- Mouvement du héros ----

    const float dt = 0.1f;
    const float dampingX = 0.5f;
    const float dampingY = 0.6f;

    Vector2 totalForce = ComputeForces(proximityData, dims);

    GLOBAL_HERO_VELOCITY.x = (GLOBAL_HERO_VELOCITY.x + dt * totalForce.x) * dampingX;
    GLOBAL_HERO_VELOCITY.y = (GLOBAL_HERO_VELOCITY.y + dt * totalForce.y) * dampingY;

    GLOBAL_HERO_POS.x += GLOBAL_HERO_VELOCITY.x;
    GLOBAL_HERO_POS.y += GLOBAL_HERO_VELOCITY.y;

    if (IsKeyDown(KEY_RIGHT))
    {
        GLOBAL_HERO_ROTATION += 5.0f;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        GLOBAL_HERO_ROTATION -= 5.0f;
    }

    //POUR TEST -- A REMPLACER PAR CALCUL DEPUIS LES FORCES//
    /*if (IsKeyDown(KEY_RIGHT))
    {
        GLOBAL_HERO_POS.x = GLOBAL_HERO_POS.x + 0.5f;
        GLOBAL_HERO_ROTATION = GLOBAL_HERO_ROTATION + 5.0f;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        GLOBAL_HERO_POS.x = GLOBAL_HERO_POS.x - 0.5f;
        GLOBAL_HERO_ROTATION = GLOBAL_HERO_ROTATION - 5.0f;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        GLOBAL_HERO_POS.y = GLOBAL_HERO_POS.y + 0.5f;
        GLOBAL_HERO_ROTATION = GLOBAL_HERO_ROTATION + 5.0f;
    }
    if (IsKeyDown(KEY_UP))
    {
        GLOBAL_HERO_POS.y = GLOBAL_HERO_POS.y - 0.5f;
        GLOBAL_HERO_ROTATION = GLOBAL_HERO_ROTATION - 5.0f;
    }*/
    //POUR TEST -- A SUPPRIMER//
}