#include "bruxa.h"
#include <math.h>

#define INTERVALO_APARICAO_BRUXA 2.7f   // segundos entre aparições
#define INTERVALO_TIRO_BRUXA     1.8f   // segundos entre tiros
#define VELOCIDADE_TIRO_BRUXA    280.0f
#define VELOCIDADE_TIRO_ROTACAO  250.0f // giro visual do projétil

// ─────────────────────────────────────────────────────────────
// Auxiliar: normaliza um vetor 2D
// ─────────────────────────────────────────────────────────────
static Vector2 NormalizarVetor(Vector2 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len == 0) return (Vector2){1, 0};
    return (Vector2){v.x / len, v.y / len};
}

// ─────────────────────────────────────────────────────────────
void InitBruxa(Bruxa *b) {
    b->ativa                = false;
    b->atirando             = false;
    b->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
    b->tempoAtirando        = 0.0f;
    b->tirosDados           = 0;
    b->tempoAnimAtaque = 0.0f;
    b->vidas = 3;

    // Posição inicial fora da tela (direita, área aérea)
    b->posicao   = (Vector2){-200, 0};
    b->velocidade = (Vector2){-120, 0}; // move para a esquerda

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        b->tiros[i].ativo = false;
    }

    b->texVoando  = LoadTexture("imagens/bruxa_voando.png");
    b->texAtirando = LoadTexture("imagens/bruxa_atirando.png");
    b->texTiro    = LoadTexture("imagens/tiro_bruxa.png");
}

// ─────────────────────────────────────────────────────────────
void UpdateBruxa(Bruxa *b, Vector2 posJogador, int largura, int altura) {

    float dt = GetFrameTime();

    // ── APARIÇÃO ──────────────────────────────────────────────
    if (!b->ativa) {
        b->tempoCooldownAparicao -= dt;
        if (b->tempoCooldownAparicao <= 0) {
            float yAereo = (float)GetRandomValue((int)(altura * 0.15f),
                                                  (int)(altura * 0.45f));
            b->posicao       = (Vector2){(float)largura + 60, yAereo};
            b->velocidade    = (Vector2){-170, 0};
            b->ativa         = true;
            b->atirando      = false;
            b->tirosDados    = 0;
            b->tempoAtirando = 0.0f;
            b->vidas = 3;
        }
        goto atualizar_tiros; 
    }

    // ── ANIMAÇÃO DE ATAQUE ────────────────
    if (b->atirando) {
        b->tempoAnimAtaque += dt;
        if (b->tempoAnimAtaque >= 0.4f) {
            b->atirando        = false;
            b->tempoAnimAtaque = 0.0f;
        }
    }

    // ── MOVIMENTO ─────────────────────────────────────────────
    b->posicao.x += b->velocidade.x * dt;
    // Saiu pela esquerda → desaparece e inicia cooldown
    if (b->posicao.x + 120 < 0) {
        b->ativa = false;
        b->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
        goto atualizar_tiros;
    }

    // ── DISPARO ───────────────────────────────────────────────
    if (b->tirosDados < MAX_TIROS_BRUXA) {
        b->tempoAtirando += dt;
        if (b->tempoAtirando >= INTERVALO_TIRO_BRUXA) {
            b->tempoAtirando = 0.0f;

            // Procura slot livre
            for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
                if (!b->tiros[i].ativo) {
                    Vector2 dir = {
                        posJogador.x - b->posicao.x,
                        posJogador.y - b->posicao.y
                    };
                    dir = NormalizarVetor(dir);

                    b->tiros[i].ativo    = true;
                    b->tiros[i].posicao  = b->posicao;
                    b->tiros[i].velocidade = (Vector2){
                        dir.x * VELOCIDADE_TIRO_BRUXA,
                        dir.y * VELOCIDADE_TIRO_BRUXA
                    };
                    b->tiros[i].rotacao  = 0.0f;
                    b->tirosDados++;

                    // Fica na textura "atirando" por 0.3 s
                    b->atirando = true;
                    break;
                }
            }
        }
        // Volta para textura de voo após breve animação
        if (b->atirando && b->tempoAtirando >= 0.3f) {
            b->atirando = false;
        }
    }

atualizar_tiros:;
    // ── ATUALIZAÇÃO DOS TIROS ─────────────────────────────────
    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!b->tiros[i].ativo) continue;

        b->tiros[i].posicao.x += b->tiros[i].velocidade.x * dt;
        b->tiros[i].posicao.y += b->tiros[i].velocidade.y * dt;

        // Saiu da tela → desativa
        if (b->tiros[i].posicao.x < -40  || b->tiros[i].posicao.x > largura  + 40 ||
            b->tiros[i].posicao.y < -40  || b->tiros[i].posicao.y > altura + 40) {
            b->tiros[i].ativo = false;
        }
    }

}

// ─────────────────────────────────────────────────────────────
void DrawBruxa(Bruxa *b) {
    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!b->tiros[i].ativo) continue;
        Texture2D t = b->texTiro;
        DrawTexturePro(
            t,
            (Rectangle){0, 0, t.width, t.height},
            (Rectangle){b->tiros[i].posicao.x, b->tiros[i].posicao.y, 30, 30},
            (Vector2){15, 15},
            0.0f,
            WHITE
        );
    }

    if (!b->ativa) return;

    Texture2D texAtual = b->atirando ? b->texAtirando : b->texVoando;
    DrawTexturePro(
        texAtual,
        (Rectangle){0, 0, texAtual.width, texAtual.height},
        (Rectangle){b->posicao.x, b->posicao.y, 120, 120}, 
        (Vector2){60, 60},                                   
        0.0f,
        WHITE
    );

    // fundo da barra
    DrawRectangle(
        b->posicao.x - 30,
        b->posicao.y - 85,
        60,
        8,
        DARKGRAY
    );

    // vida atual
    DrawRectangle(
        b->posicao.x - 30,
        b->posicao.y - 85,
        20 * b->vidas,
        8,
        RED
    );
}

// ─────────────────────────────────────────────────────────────
void UnloadBruxa(Bruxa *b) {
    UnloadTexture(b->texVoando);
    UnloadTexture(b->texAtirando);
    UnloadTexture(b->texTiro);
}
