#include "objetos.h"
#include <stdlib.h>

#define TAMANHO_SPIKE 42
#define TEMPO_SPAWN_SPIKE 2.0f

typedef struct NoSpike {
    Vector2 position;
    float speed;
    struct NoSpike *next;
} NoSpike;

static NoSpike *listaSpikes = NULL;
static float timerSpawnSpike = 0;

static void CriarSpike(int largura, int altura) {
    NoSpike *novo = (NoSpike *)malloc(sizeof(NoSpike));

    if (novo == NULL) return;

    novo->position.x = largura + GetRandomValue(80, 260);
    novo->position.y = GetRandomValue((int)(altura * 0.25f), (int)(altura * 0.75f));
    novo->speed = GetRandomValue(140, 180);
    novo->next = listaSpikes;

    listaSpikes = novo;
}

static void LimparListaSpikes(void) {
    NoSpike *atual = listaSpikes;

    while (atual != NULL) {
        NoSpike *temp = atual;
        atual = atual->next;
        free(temp);
    }

    listaSpikes = NULL;
}

void InitSpike(SpikeBall *spike) {
    spike->position = (Vector2){1280, 520};
    spike->speed = 180;
    spike->active = false;
    spike->texture = LoadTexture("imagens/spike.png");

    LimparListaSpikes();
    timerSpawnSpike = 0;
}

void UpdateSpike(SpikeBall *spike) {
    spike->position.x -= spike->speed * GetFrameTime();

    if (spike->position.x < -60) {
        spike->position.x = 1280;
    }
}

void UpdateSpikeAleatorio(SpikeBall *spike, int largura, int altura) {
    if (!spike->active) return;

    float dt = GetFrameTime();

    timerSpawnSpike += dt;

    if (timerSpawnSpike >= TEMPO_SPAWN_SPIKE) {
        CriarSpike(largura, altura);
        timerSpawnSpike = 0;
    }

    NoSpike *atual = listaSpikes;
    NoSpike *anterior = NULL;

    while (atual != NULL) {
        atual->position.x -= atual->speed * dt;

        if (atual->position.x < -80) {
            NoSpike *remover = atual;

            if (anterior == NULL) {
                listaSpikes = atual->next;
                atual = listaSpikes;
            } else {
                anterior->next = atual->next;
                atual = anterior->next;
            }

            free(remover);
        } else {
            anterior = atual;
            atual = atual->next;
        }
    }
}

void DrawSpike(SpikeBall spike) {
    if (!spike.active) return;

    NoSpike *atual = listaSpikes;

    while (atual != NULL) {
        DrawTexturePro(
            spike.texture,
            (Rectangle){0, 0, spike.texture.width, spike.texture.height},
            (Rectangle){atual->position.x, atual->position.y, TAMANHO_SPIKE, TAMANHO_SPIKE},
            (Vector2){TAMANHO_SPIKE / 2.0f, TAMANHO_SPIKE / 2.0f},
            0,
            WHITE
        );

        atual = atual->next;
    }
}

bool CheckSpikeCollision(SpikeBall spike, Rectangle playerRect) {
    if (!spike.active) return false;

    NoSpike *atual = listaSpikes;
    NoSpike *anterior = NULL;

    while (atual != NULL) {
        Rectangle spikeRect = {
            atual->position.x - 18,
            atual->position.y - 18,
            36,
            36
        };

        if (CheckCollisionRecs(spikeRect, playerRect)) {
            NoSpike *remover = atual;

            if (anterior == NULL) {
                listaSpikes = atual->next;
            } else {
                anterior->next = atual->next;
            }

            free(remover);
            return true;
        }

        anterior = atual;
        atual = atual->next;
    }

    return false;
}

void UnloadSpike(SpikeBall *spike) {
    LimparListaSpikes();
    UnloadTexture(spike->texture);
}

