#ifndef DRAWHERO_H
#define DRAWHERO_H

#include "raylib.h"
#include "Variables.h"
#include "RenderUtils.h"
#include <vector>

void DrawHero(const std::vector<POLYLINE>& polylines, RenderDims dims);

#endif
