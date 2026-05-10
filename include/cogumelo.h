#ifndef COGUMELO_H
#define COGUMELO_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector2 posicao;
    float velocidade;
    bool ativo;
    float tempoCooldownAparicao;
    Texture2D textura;
    int vidas;
} CogumeloRei;

void InitCogumelo(CogumeloRei *c, int largura, float chaoY);
void UpdateCogumelo(CogumeloRei *c, int largura, float chaoY);
void DrawCogumelo(CogumeloRei *c);
bool CheckCogumeloCollision(CogumeloRei *c, Rectangle playerRect);
void UnloadCogumelo(CogumeloRei *c);

#endif
