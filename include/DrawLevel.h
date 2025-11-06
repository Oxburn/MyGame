#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H

#include "raylib.h"
#include "RenderUtils.h"
#include <vector>

void DrawLevel(const std::vector<Vector2>& points, float thickness, Color boundaryColor, Color surfaceColor, RenderDims dims);

#endif