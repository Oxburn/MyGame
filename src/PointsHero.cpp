#include "raylib.h"
#include "Variables.h"
#include <vector>
#include <cmath>


int GLOBAL_HERO_ACCURACY = 30;      // Finesse de représentation du héros


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