#ifndef TIRO_H
#define TIRO_H

#include "raylib.h"

void InitTiros();
void DispararTiro(Vector2 origem, Vector2 destino);
void AtualizarTiros(float dt, int largura, int altura);
void DesenharTiros(Texture2D textura);

#endif