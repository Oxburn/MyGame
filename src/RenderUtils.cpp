#include "Variables.h"
#include "RenderUtils.h"

struct RenderDims {
    int drawWidth;
    int drawHeight;
    int offsetX;
    int offsetY;
};

RenderDims ComputeRenderDims()
{
    int windowWidth = GetScreenWidth();
    int windowHeight = GetScreenHeight();
    float currentRatio = (float)windowWidth / windowHeight;

    int drawWidth = windowWidth;
    int drawHeight = windowHeight;

    if (currentRatio > WINDOW_RATIO)
        drawWidth = (int)(windowHeight * WINDOW_RATIO);
    else if (currentRatio < WINDOW_RATIO)
        drawHeight = (int)(windowWidth / WINDOW_RATIO);

    int offsetX = (windowWidth - drawWidth) / 2;
    int offsetY = (windowHeight - drawHeight) / 2;

    return RenderDims{drawWidth, drawHeight, offsetX, offsetY};
}

void DrawRenderTextureToScreen()
{
    RenderDims dims = ComputeRenderDims();

    BeginDrawing();
        ClearBackground(NO_DRAW_COLOR);
        Rectangle srcRect = { 0.0f, 0.0f,
                              (float)GLOBAL_RENDER_TEXTURE.texture.width,
                             -(float)GLOBAL_RENDER_TEXTURE.texture.height };
        Rectangle destRect = { (float)dims.offsetX, (float)dims.offsetY,
                               (float)dims.drawWidth, (float)dims.drawHeight };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(GLOBAL_RENDER_TEXTURE.texture, srcRect, destRect, origin, 0.0f, TEXTURE_PRO_COLOR());
    EndDrawing();
}
