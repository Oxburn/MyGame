#include "raylib.h"
#include "raymath.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>
#include <cmath>
#include <algorithm> 
#include <numeric>


// Paramètres
float GLOBAL_MAX_SPEED = 3.0f;
float GLOBAL_HERO_ACCELERATION = 0.5f;
float GLOBAL_HERO_BOUNCE = 1.0f;      // Ecrasement du héros (0.5 -> Très écrasé / 5.0f -> Peu écrasé)
Vector2 GLOBAL_GRAVITY_FORCE = {0.0f, 7.0f};
float GLOBAL_JUMP_POWER = 15.0f; // 8.0f (un peu plus que la gravité) -> Saut minimal
Vector2 NO_GLITCH_POINT = {0, 0}; //
bool GLOBAL_HERO_GLITCHED = false;
int GLOBAL_HERO_ACCURACY = 50;      // Finesse de représentation du héros


std::vector<std::pair<float, Vector2>> PointsHero()
{
    // ---- Liste à remplir des points de passage du personnage ----

    std::vector<std::pair<float, Vector2>> points;


    // ---- Boucle ----

    for (int i = 0; i < GLOBAL_HERO_ACCURACY; i++)
    {
        // ---- Angle fonction de la rotation courante du personnage ----

        float angle = (2.0f * PI * i + GLOBAL_HERO_ROTATION) / GLOBAL_HERO_ACCURACY;


        // ---- Forme de base = cercle avec respect du ratio ----

        float posX = GLOBAL_HERO_POS.x + GLOBAL_HERO_RADIUS * cosf(angle);
        posX = GLOBAL_HERO_POS.x + (posX - GLOBAL_HERO_POS.x) / WINDOW_RATIO;
        float posY = GLOBAL_HERO_POS.y + GLOBAL_HERO_RADIUS * sinf(angle);
        Vector2 point = { posX, posY };

        points.push_back( {angle, point} );
    }

    return points;
}

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

bool IsPointInsideTriangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c)
{
    float areaOrig = fabsf((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
    float area1 = fabsf((a.x - p.x) * (b.y - p.y) - (b.x - p.x) * (a.y - p.y));
    float area2 = fabsf((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y));
    float area3 = fabsf((c.x - p.x) * (a.y - p.y) - (a.x - p.x) * (c.y - p.y));
    float sum = area1 + area2 + area3;

    return fabsf(sum - areaOrig) < 0.01f; // tolérance flottante
}

Vector2 ClosestPointOnBoundary(Vector2 p, const std::vector<Vector2>& boundaryPoints)
{
    float minDist = 1e6;
    Vector2 unglitchedPos = NO_GLITCH_POINT;

    for (size_t i = 0; i < boundaryPoints.size() - 1; i++)
    {
        Vector2 A = boundaryPoints[i];
        Vector2 B = boundaryPoints[i + 1];
        Vector2 AB = Vector2Subtract(B, A);
        float l2 = Vector2LengthSqr(AB);

        // Projection du point p sur le segment [A, B]
        float t = fmaxf(0.0f, fminf(1.0f, Vector2DotProduct(Vector2Subtract(p, A), AB) / l2));
        Vector2 projection = { A.x + t * AB.x, A.y + t * AB.y };

        // Distance au segment
        float dist = Vector2Distance(p, projection);

        if (dist < minDist)
        {
            minDist = dist;
            unglitchedPos = projection;
        }
    }

    return unglitchedPos;
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

Vector2 ComputeForces(std::vector<Vector2> proximityData)
{
    // ---- Forces de réaction ----

    std::vector<Vector2> reactionForces;
    for (size_t i = 0; i < proximityData.size(); i++)
    {
        float angle = proximityData[i].x;
        float distance = proximityData[i].y;

        if (distance < GLOBAL_HERO_RADIUS) {

            float penetration = GLOBAL_HERO_RADIUS - distance;
            float totalForce = GLOBAL_HERO_BOUNCE * penetration;

            float reactionForceX = totalForce * cos(angle + PI);
            float reactionForceY = totalForce * sin(angle + PI);
            Vector2 reactionForce = {reactionForceX, reactionForceY};
            reactionForces.push_back(reactionForce);
        }
    }


    // ---- Forces du saut (proportionnelles aux Forces de réaction !) ----

    std::vector<Vector2> jumpForces;
    if (!reactionForces.empty())
    {
        if (IsKeyPressed(KEY_S) and !(GLOBAL_HERO_GLITCHED))
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
            for (size_t i = 0; i < reactionForces.size() - 1; i++)
            {
                Vector2 reactionForce = reactionForces[i];
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
                jumpForces.push_back(jumpForce);
            }
        }
    }


    // ---- Force de gravité ----

    std::vector<Vector2> gravityForces;
    gravityForces.push_back(GLOBAL_GRAVITY_FORCE);
    
    
    // ---- Force de colle (Contre-forces de réaction et zéro gravité) ----
    
    std::vector<Vector2> stickForces;

    float sumX = 0.0f;
    float sumY = 0.0f;
    for (const auto& reactionForce : reactionForces)
    {
        float forceX = reactionForce.x;
        float forceY = reactionForce.y;

        sumX += forceX;
        sumY += forceY;
    }
    
    bool canStick;
    //TraceLog(LOG_NONE, "ReactionX : %.2f", std::abs(sumX));
    //TraceLog(LOG_NONE, "ReactionY : %.2f", sumY);
    //TraceLog(LOG_NONE, "abs(X) + Y : %.2f", std::abs(sumX) + sumY);

    //if ((std::abs(sumX) > 1.0f or sumY > 1.0f))
    if ((std::abs(sumX) < 0.1f and sumY > 1.0f) or //Plafond
        (std::abs(sumX) > 1.0f and sumY < 0.1f) or //Mur
        (std::abs(sumX) > 0.2f and sumY > 0.2f)) //Coin plafond
    {
        canStick = true;
    }
    else
    {
        canStick = false;
        GLOBAL_START_STICK = GetTime();
        GLOBAL_STICK_INTENSITY = 1.0f;
    }
    
    if (canStick)
    {
        if (GLOBAL_STICK_INTENSITY > 0.0f)
        {
            float stickTime = GetTime() - GLOBAL_START_STICK;
            stickTime = std::max(stickTime, 0.0f);
            if (stickTime <= GLOBAL_STICK_DURATION)
            {
                //TraceLog(LOG_NONE, "Doit coller %.2f", stickTime);
                float decreasing = stickTime / GLOBAL_STICK_DURATION;
                GLOBAL_STICK_INTENSITY -= GLOBAL_STICK_INTENSITY * decreasing;
                gravityForces.clear();
                
                std::vector<Vector2> newForces;
                for (const auto& reactionForce : reactionForces)
                {
                    float forceX = reactionForce.x;
                    float forceY = reactionForce.y;

                    float stickForceY = GLOBAL_STICK_INTENSITY * forceY;

                    stickForces.push_back( {0.0f, stickForceY} );
                    newForces.push_back({
                        forceX * 0.15f,
                        forceY * 0.05f
                    });
                }

                reactionForces = newForces;
            }
        }
    }


    // ---- Forces de déplacement ----

    std::vector<Vector2> moveForces;
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
    moveForces.push_back(GLOBAL_HERO_MOVE);


    // ---- Bilan des forces ----

    std::vector<std::vector<Vector2>> allForces = {
        reactionForces,
        jumpForces,
        gravityForces,
        stickForces,
        moveForces
    };
    
    std::vector<Vector2> forces;
    forces.reserve(
        std::accumulate(allForces.begin(), allForces.end(), 0u,
            [](size_t sum, const std::vector<Vector2>& v) { return sum + v.size(); })
    );

    for (const auto& vec : allForces) {
        forces.insert(forces.end(), vec.begin(), vec.end());
    }

    float totalForceX = 0.0f;
    float totalForceY = 0.0f;
    for (const auto& force : forces)
    {
        totalForceX += force.x;
        totalForceY += force.y;
    }


    // ---- Dessin des vecteurs de force (à supprimer à termes ?) ----

    for (size_t i = 0; i < forces.size(); i++)
    {
        Vector2 start = {
            (float)(GLOBAL_HERO_POS.x * GLOBAL_RENDER_WIDTH / 100),
            (float)(GLOBAL_HERO_POS.y * GLOBAL_RENDER_HEIGHT / 100)
        };

        Vector2 end = {
            (float)((GLOBAL_HERO_POS.x + forces[i].x) * GLOBAL_RENDER_WIDTH / 100),
            (float)((GLOBAL_HERO_POS.y + forces[i].y) * GLOBAL_RENDER_HEIGHT / 100)
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


    return {totalForceX, totalForceY};
}


void DrawHero(std::vector<LEVEL_DEFINITION>& levelDefinitions)
{
    // ---- Dessin du héros ----

    std::vector<Vector2> proximityData = GetProximity(levelDefinitions);
    int sideContact = 1;
    float contactDistance = GLOBAL_HERO_RADIUS;

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

        if (startDistance < contactDistance)
        {
            contactDistance = startDistance;
            if (startDistance * cosf(startAngle) < startDistance * sinf(startAngle))
            {
                sideContact = 1;
            }
            else
            {
                sideContact = -1;
            }

        }

        startPtX = GLOBAL_HERO_POS.x + (startPtX - GLOBAL_HERO_POS.x) / WINDOW_RATIO;
        endPtX = GLOBAL_HERO_POS.x + (endPtX - GLOBAL_HERO_POS.x) / WINDOW_RATIO;

        Vector2 startPt = { 
            (float)(startPtX * GLOBAL_RENDER_WIDTH / 100),
            (float)(startPtY * GLOBAL_RENDER_HEIGHT / 100)
        };
        Vector2 endPt = { 
            (float)(endPtX * GLOBAL_RENDER_WIDTH / 100),
            (float)(endPtY * GLOBAL_RENDER_HEIGHT / 100)
        };

        float t = (float)i / (float)(proximityData.size() - 1); // de 0.0 à 1.0
        unsigned char r = (unsigned char)(80 + t * 60);   // de 80 à 140
        unsigned char g = (unsigned char)(120 + t * 80);  // de 120 à 200
        unsigned char b = (unsigned char)(200 + t * 55); // de 150 à 255
        Color heroColor = {r, g, b, 255};

        DrawLineEx(startPt, endPt, 2.0f, heroColor);
    }
    

    // ---- Mouvement du héros ----

    const float dt = 0.1f;
    const float dampingX = 0.5f;
    const float dampingY = 0.6f;

    Vector2 totalForce = ComputeForces(proximityData);

    GLOBAL_HERO_VELOCITY.x = (GLOBAL_HERO_VELOCITY.x + dt * totalForce.x) * dampingX;
    GLOBAL_HERO_VELOCITY.y = (GLOBAL_HERO_VELOCITY.y + dt * totalForce.y) * dampingY;


    // ---- Animation du héros ----

    // Physique (forces, vitesse)
    GLOBAL_HERO_POS.x += GLOBAL_HERO_VELOCITY.x;
    GLOBAL_HERO_POS.y += GLOBAL_HERO_VELOCITY.y;

    // Corriger si pénétration
    GLOBAL_HERO_GLITCHED = false;
    for (const auto& levelDefinition : levelDefinitions)
    {
        for (const auto& innerTriangle : levelDefinition.innerSurface)
        {
            Vector2 A = innerTriangle[0];
            Vector2 B = innerTriangle[1];
            Vector2 C = innerTriangle[2];

            if (IsPointInsideTriangle(GLOBAL_HERO_POS, A, B, C))
            {
                GLOBAL_HERO_GLITCHED = true;
                float triangleArea = 0.5f * fabsf((B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x));
                if (triangleArea > 0.01f)
                {
                    std::vector<Vector2> boundaryPoints = levelDefinition.boundaryPoints;
                    Vector2 unglitchedPos = ClosestPointOnBoundary(GLOBAL_HERO_POS, boundaryPoints);

                    if (!(unglitchedPos == NO_GLITCH_POINT))
                    {
                        Vector2 dir = Vector2Normalize(Vector2Subtract(unglitchedPos, GLOBAL_HERO_POS));
                        unglitchedPos = Vector2Add(unglitchedPos, Vector2Scale(dir, 1.0f));
                        DrawCircle(
                            unglitchedPos.x * GLOBAL_RENDER_WIDTH / 100,
                            unglitchedPos.y * GLOBAL_RENDER_HEIGHT / 100,
                            10.0f, YELLOW);
                        GLOBAL_HERO_POS = unglitchedPos;
                    }
                }
            }
        }
    }


    // Rotation
    if (IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_DOWN))
    {
        GLOBAL_HERO_ROTATION += sideContact * contactDistance * 5.0f;
    }
    if (IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_UP))
    {
        GLOBAL_HERO_ROTATION -= sideContact * contactDistance * 5.0f;
    }
}