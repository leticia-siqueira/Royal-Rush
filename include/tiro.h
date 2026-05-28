#ifndef TIRO_H
#define TIRO_H

#include "raylib.h"
#include <stdbool.h>

void InitTiros(void);
int ContarTirosAtivos(void);
void DispararTiro(Vector2 origem, Vector2 destino);
void AtualizarTiros(float dt, int largura, int altura);
void DesenharTiros(Texture2D textura);

int GetMaxTiros(void);

void GetTiroRect(
    int indiceTiro,
    Rectangle *hitboxTiro,
    bool *tiroEstaAtivo
);

void DesativarTiro(int indiceTiro);

#endif

