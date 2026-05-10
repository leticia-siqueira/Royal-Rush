#include "tiro.h"
#include <math.h>

#define MAX_TIROS 50

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
    float rotacao;
} Tiro;

static Tiro tiros[MAX_TIROS];

void InitTiros(void) {
    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }
}

void DispararTiro(Vector2 origem, Vector2 destino) {
    Vector2 direcao = {
        destino.x - origem.x,
        destino.y - origem.y
    };
    float tamanho = sqrt(direcao.x * direcao.x + direcao.y * direcao.y);
    if (tamanho == 0) return;
    direcao.x /= tamanho;
    direcao.y /= tamanho;

    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) {
            tiros[i].ativo       = true;
            tiros[i].posicao     = origem;
            tiros[i].rotacao     = 0;
            tiros[i].velocidade  = (Vector2){direcao.x * 300, direcao.y * 300};
            break;
        }
    }
}

void AtualizarTiros(float dt, int largura, int altura) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;
        tiros[i].posicao.x += tiros[i].velocidade.x * dt;
        tiros[i].posicao.y += tiros[i].velocidade.y * dt;
        tiros[i].rotacao   += 200 * dt;
        if (tiros[i].posicao.x < 0 || tiros[i].posicao.x > largura ||
            tiros[i].posicao.y < 0 || tiros[i].posicao.y > altura) {
            tiros[i].ativo = false;
        }
    }
}

void DesenharTiros(Texture2D textura) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;
        DrawTexturePro(
            textura,
            (Rectangle){0, 0, textura.width, textura.height},
            (Rectangle){tiros[i].posicao.x, tiros[i].posicao.y, 30, 30},
            (Vector2){15, 15},
            tiros[i].rotacao,
            WHITE
        );
    }
}

// ── Auxiliares públicos ─────────────────────────────────────

int GetMaxTiros(void) {
    return MAX_TIROS;
}

void GetTiroRect(int index, Rectangle *out, bool *ativo) {
    if (index < 0 || index >= MAX_TIROS) {
        *ativo = false;
        return;
    }
    *ativo = tiros[index].ativo;
    // Hitbox centralizada de 30x30 (mesmo tamanho do draw)
    out->x      = tiros[index].posicao.x - 15;
    out->y      = tiros[index].posicao.y - 15;
    out->width  = 30;
    out->height = 30;
}

void DesativarTiro(int index) {
    if (index >= 0 && index < MAX_TIROS) {
        tiros[index].ativo = false;
    }
}
