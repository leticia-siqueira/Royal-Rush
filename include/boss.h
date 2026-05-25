#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_TIROS_BOSS 5

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
    float rotacao;
} TiroBoss;

typedef struct {
    Vector2 posicao;
    float velocidadeY;

    int vidas;
    int vidaMaxima;

    bool ativo;
    bool atirando;

    float tempoTiro;
    float tempoAnimAtaque;
    float tempoAnimIdle;

    Texture2D texIdle1;
    Texture2D texIdle2;
    Texture2D texAtirando;
    Texture2D texTiro;

    TiroBoss tiros[MAX_TIROS_BOSS];
} Boss;

void InitBoss(Boss *boss, int largura, int altura);
void UpdateBoss(Boss *boss, Vector2 posJogador, int largura, int altura);
void DrawBoss(Boss *boss);
void UnloadBoss(Boss *boss);

Rectangle GetBossRect(Boss *boss);
void BossReceberDano(Boss *boss, int dano);
bool BossFoiDerrotado(Boss *boss);
bool CheckBossTirosCollision(Boss *boss, Rectangle playerRect);

#endif

