#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

// CONFIG
#define GRAVIDADE 400
#define FORCA_PULO 450.0f
#define VELOCIDADE_HORIZONTAL 200.0f

typedef enum {
    IDLE,
    RUN,
    JUMP
} EstadoJogador;

typedef struct {
    Rectangle area;
    bool bloqueia;
    Color cor;
} Plataforma;

typedef struct {
    Vector2 posicao;
    float velocidadeY;
    bool podePular;
    int vidas;
    EstadoJogador estado;
} Jogador;

void AtualizarJogador(Jogador *jogador, Plataforma *plataformas, int qtd, float dt);

#endif