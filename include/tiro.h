#ifndef TIRO_H
#define TIRO_H

#include "raylib.h"
#include <stdbool.h>

void InitTiros();
int ContarTirosAtivos();
void DispararTiro(Vector2 origem, Vector2 destino);
void AtualizarTiros(float dt, int largura, int altura);
void DesenharTiros(Texture2D textura);


#endif
