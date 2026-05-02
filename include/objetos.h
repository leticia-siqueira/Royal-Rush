#ifndef OBJETOS_H
#define OBJETOS_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector2 position;
    float speed;
    Texture2D texture;
    bool active;
} SpikeBall;

void InitSpike(SpikeBall *spike);
void UpdateSpike(SpikeBall *spike);
void DrawSpike(SpikeBall spike);
bool CheckSpikeCollision(SpikeBall spike, Rectangle playerRect);
void UnloadSpike(SpikeBall *spike);

#endif

