#include "cogumelo.h"

#define INTERVALO_APARICAO_COGUMELO 2.0f // segundos entre aparições
#define LARGURA_COGUMELO 95
#define ALTURA_COGUMELO  95

// ─────────────────────────────────────────────────────────────
void InitCogumelo(CogumeloRei *c, int largura, float chaoY) {
    c->ativo                 = false;
    c->tempoCooldownAparicao = INTERVALO_APARICAO_COGUMELO;
    c->velocidade            = 160.0f;
    c->posicao               = (Vector2){(float)largura + 80,
                                          chaoY - ALTURA_COGUMELO / 2.0f};
    c->textura = LoadTexture("imagens/cogumelo_rei.png");
    c->vidas = 2;
}

// ─────────────────────────────────────────────────────────────
void UpdateCogumelo(CogumeloRei *c, int largura, float chaoY) {
    float dt = GetFrameTime();

    if (!c->ativo) {
        c->tempoCooldownAparicao -= dt;
        if (c->tempoCooldownAparicao <= 0) {
            // Spawna pela direita, parado no chão
            c->posicao = (Vector2){(float)largura + 80,
                                    chaoY - ALTURA_COGUMELO / 2.0f};
            c->ativo   = true;
            c->vidas = 2;
        }
        return;
    }

    // Anda para a esquerda
    c->posicao.x -= c->velocidade * dt;

    // Saiu pela esquerda → desaparece e inicia cooldown
    if (c->posicao.x + LARGURA_COGUMELO < 0) {
        c->ativo                 = false;
        c->tempoCooldownAparicao = INTERVALO_APARICAO_COGUMELO;
    }
}

// ─────────────────────────────────────────────────────────────
void DrawCogumelo(CogumeloRei *c) {
    if (!c->ativo) return;

    DrawTexturePro(
        c->textura,
        (Rectangle){0, 0, c->textura.width, c->textura.height},
        (Rectangle){c->posicao.x, c->posicao.y, LARGURA_COGUMELO, ALTURA_COGUMELO},
        (Vector2){LARGURA_COGUMELO / 2.0f, ALTURA_COGUMELO / 2.0f},
        0.0f,
        WHITE
    );

        // fundo da barra
        DrawRectangle(
        c->posicao.x - 30,
        c->posicao.y - 85,
        60,
        8,
        DARKGRAY
    );

    // vida atual
    DrawRectangle(
        c->posicao.x - 30,
        c->posicao.y - 85,
        30 * c->vidas,
        8,
        RED
    );
}

// ─────────────────────────────────────────────────────────────
bool CheckCogumeloCollision(CogumeloRei *c, Rectangle playerRect) {
    if (!c->ativo) return false;

    Rectangle cogumeloRect = {
        c->posicao.x - LARGURA_COGUMELO / 2.0f,
        c->posicao.y - ALTURA_COGUMELO  / 2.0f,
        LARGURA_COGUMELO,
        ALTURA_COGUMELO
    };

    return CheckCollisionRecs(cogumeloRect, playerRect);
}

// ─────────────────────────────────────────────────────────────
void UnloadCogumelo(CogumeloRei *c) {
    UnloadTexture(c->textura);
}
