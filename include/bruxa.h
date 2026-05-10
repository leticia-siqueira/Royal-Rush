#ifndef BRUXA_H
#define BRUXA_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_TIROS_BRUXA 4

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
    float rotacao;
} TiroBruxa;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float tempoCooldownAparicao; // tempo até aparecer novamente após sumir
    float tempoAtirando; 
    float tempoAnimAtaque;        // tempo desde o último tiro
    int tirosDados;              // quantos tiros deu nesta aparição
    bool ativa;
    bool atirando;               // true = usar textura de ataque

    TiroBruxa tiros[MAX_TIROS_BRUXA];

    Texture2D texVoando;
    Texture2D texAtirando;
    Texture2D texTiro;
} Bruxa;

void InitBruxa(Bruxa *b);
void UpdateBruxa(Bruxa *b, Vector2 posJogador, int largura, int altura);
void DrawBruxa(Bruxa *b);
void UnloadBruxa(Bruxa *b);

#endif
