#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H

#include "raylib.h"
#include "RenderUtils.h"
#include <vector>

std::vector<LEVEL_PART_DEFINITION> BoundaryPointsPositioned(const std::vector<LEVEL_PART_DEFINITION> levelPartDefinitions);
std::vector<std::vector<Vector2>> CurveParallel (const std::vector<Vector2>& boundaryPoints, float distance);
std::vector<std::vector<Vector2>> SurfaceInner (const std::vector<Vector2>& boundaryPoints, std::vector<std::vector<Vector2>>& parallelCurve);
void DrawLevel(LEVEL_DEFINITION levelDefinition);

#endif