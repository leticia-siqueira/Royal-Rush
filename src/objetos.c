#include "objetos.h"

void InitSpike(SpikeBall *spike) {
    spike->position = (Vector2){1280, 520}; 
    spike->speed = 150;
    spike->active = true;

    spike->texture = LoadTexture("imagens/spike.png");
}

void UpdateSpike(SpikeBall *spike) {
    spike->position.x -= spike->speed * GetFrameTime();

    if (spike->position.x < -60) {
        spike->position.x = 1280;
    }
}

void DrawSpike(SpikeBall spike) {
    DrawTexturePro(
        spike.texture,
        (Rectangle){0, 0, spike.texture.width, spike.texture.height},
        (Rectangle){spike.position.x, spike.position.y, 60, 60},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

bool CheckSpikeCollision(SpikeBall spike, Rectangle playerRect) {
    Rectangle spikeRect = {
        spike.position.x,
        spike.position.y,
        60,
        60
    };

    return CheckCollisionRecs(spikeRect, playerRect);
}

void UnloadSpike(SpikeBall *spike) {
    UnloadTexture(spike->texture);
}

