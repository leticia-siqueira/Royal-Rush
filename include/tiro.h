#ifndef TIRO_H
#define TIRO_H

#include "raylib.h"
#include <stdbool.h>

void InitTiros(void);
void DispararTiro(Vector2 origem, Vector2 destino);
void AtualizarTiros(float dt, int largura, int altura);
void DesenharTiros(Texture2D textura);

// Auxiliares para detecção de colisão externa (usadas em main.c)
int  GetMaxTiros(void);
void GetTiroRect(int index, Rectangle *out, bool *ativo);
void DesativarTiro(int index);

#endif
