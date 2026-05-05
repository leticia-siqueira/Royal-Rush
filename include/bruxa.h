#ifndef BRUXA_H
#define BRUXA_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector2 position;
    float speed;
    Texture2D texture;
    bool active;
    float tempoCooldown;
    bool esperando;
} BruxaVoando;

void InitBruxa(BruxaVoando *bruxa);
void UpdateBruxa(BruxaVoando *bruxa);
void DrawBruxa(BruxaVoando bruxa);
bool CheckBruxaCollision(BruxaVoando bruxa, Rectangle playerRect);
void UnloadBruxa(BruxaVoando *bruxa);

#endif