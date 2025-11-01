#ifndef DRAWPOLYLINE_H
#define DRAWPOLYLINE_H

#include "raylib.h"
#include "RenderUtils.h"
#include <vector>

void DrawPolyline(const std::vector<Vector2>& points, float thickness, Color color, RenderDims dims);

#endif