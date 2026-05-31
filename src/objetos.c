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

// Esta função cria um novo spike dinamicamente usando malloc.

static void CriarSpike(int largura, int altura) {
    NoSpike *novo = (NoSpike *)malloc(sizeof(NoSpike));

    if (novo == NULL) 
        return;

    novo->position.x = largura + GetRandomValue(80, 260);
    novo->position.y = GetRandomValue((int)(altura * 0.25f), (int)(altura * 0.75f));
    novo->speed = GetRandomValue(140, 180);
    novo->next = listaSpikes;

    listaSpikes = novo;
}

// Esta função limpa todos os spikes da lista encadeada.

static void LimparListaSpikes(void) {
    NoSpike *atual = listaSpikes;

    while (atual != NULL) {
        NoSpike *temp = atual;
        atual = atual->next;
        free(temp);
    }

    listaSpikes = NULL;
}

// Esta função inicializa o sistema de spikes.

void InitSpike(SpikeBall *spike) {
    spike->position = (Vector2){1280, 520};
    spike->speed = 180;
    spike->active = false;
    spike->texture = LoadTexture("imagens/spike.png");

    LimparListaSpikes();
    timerSpawnSpike = 0;
}

// Esta função atualiza um spike simples, movendo-o para a esquerda.
// Quando ele sai da tela, sua posição é resetada e volta para a direita.

void UpdateSpike(SpikeBall *spike) {
    spike->position.x -= spike->speed * GetFrameTime();

    if (spike->position.x < -60) {
        spike->position.x = 1280;
    }
}

// Esta função é a principal lógica dos spikes aleatórios.
// Ela controla o tempo de spawn, cria novos spikes, move todos para a esquerda
// e remove da lista os spikes que saem da tela.

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

// Esta função desenha todos os spikes ativos na tela.

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

// Esta função verifica colisão entre os spikes e o jogador.
// Se colidir, o spike é removido da lista e a função retorna true.

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

// Esta função encerra o uso dos spikes.
// Ela limpa a lista encadeada e descarrega a textura da memória.

void UnloadSpike(SpikeBall *spike) {
    LimparListaSpikes();
    UnloadTexture(spike->texture);
}

