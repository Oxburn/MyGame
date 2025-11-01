#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include "raylib.h"

// Structure pour stocker les dimensions calculées
struct RenderDims {
    int drawWidth;
    int drawHeight;
    int offsetX;
    int offsetY;
    int renderTextureWidth;
    int renderTextureHeight;
    float scaleX;
};

// Calcul des dimensions de dessin respectant le ratio
RenderDims ComputeRenderDims();

void DrawRenderTextureToScreen(const RenderDims& dims);

#endif
