#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H

#include "raylib.h"
#include "RenderUtils.h"
#include <vector>

std::vector<std::vector<Vector2>> curveParallel (const std::vector<Vector2>& points, float distance);
void DrawLevel(LEVEL_DEFINITION levelDefinition);

#endif