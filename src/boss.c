#include "boss.h"
#include <math.h>

#define VIDA_BOSS 60
#define INTERVALO_TIRO_BOSS 2.2f
#define VELOCIDADE_TIRO_BOSS 330.0f

#define TAMANHO_BOSS 390
#define TAMANHO_TIRO_BOSS 95

static Vector2 Normalizar(Vector2 v) {
    float t = sqrtf(v.x * v.x + v.y * v.y);
    if (t == 0) return (Vector2){-1, 0};
    return (Vector2){v.x / t, v.y / t};
}

void InitBoss(Boss *boss, int largura, int altura) {
    boss->ativo = true;
    boss->atirando = false;
    boss->vidaMaxima = VIDA_BOSS;
    boss->vidas = VIDA_BOSS;

    boss->posicao = (Vector2){
        largura - 230,
        altura * 0.80f - 115
    };

    boss->velocidadeY = 0;
    boss->tempoTiro = 0;
    boss->tempoAnimAtaque = 0;
    boss->tempoAnimIdle = 0;

    boss->texIdle1 = LoadTexture("imagens/bossidle1.png");
    boss->texIdle2 = LoadTexture("imagens/bossidle2.png");
    boss->texAtirando = LoadTexture("imagens/bossatirando.png");
    boss->texTiro = LoadTexture("imagens/tiro_boss.png");

    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        boss->tiros[i].ativo = false;
        boss->tiros[i].rotacao = 0;
    }
}

void UpdateBoss(Boss *boss, Vector2 posJogador, int largura, int altura) {
    if (!boss->ativo) return;

    float dt = GetFrameTime();

    boss->tempoAnimIdle += dt;

    if (boss->atirando) {
        boss->tempoAnimAtaque += dt;

        if (boss->tempoAnimAtaque >= 0.50f) {
            boss->atirando = false;
            boss->tempoAnimAtaque = 0;
        }
    }

    boss->tempoTiro += dt;

    if (boss->tempoTiro >= INTERVALO_TIRO_BOSS) {
        boss->tempoTiro = 0;

        for (int i = 0; i < MAX_TIROS_BOSS; i++) {
            if (!boss->tiros[i].ativo) {
                Vector2 origem = {
                    boss->posicao.x - 185,
                    boss->posicao.y - 35
                };

                Vector2 dir = {
                    posJogador.x - origem.x,
                    posJogador.y - origem.y
                };

                dir = Normalizar(dir);

                boss->tiros[i].ativo = true;
                boss->tiros[i].posicao = origem;
                boss->tiros[i].velocidade = (Vector2){
                    dir.x * VELOCIDADE_TIRO_BOSS,
                    dir.y * VELOCIDADE_TIRO_BOSS
                };
                boss->tiros[i].rotacao = 0;

                boss->atirando = true;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        boss->tiros[i].posicao.x += boss->tiros[i].velocidade.x * dt;
        boss->tiros[i].posicao.y += boss->tiros[i].velocidade.y * dt;
        boss->tiros[i].rotacao += 180 * dt;

        if (boss->tiros[i].posicao.x < -120 || boss->tiros[i].posicao.x > largura + 120 ||
            boss->tiros[i].posicao.y < -120 || boss->tiros[i].posicao.y > altura + 120) {
            boss->tiros[i].ativo = false;
        }
    }
}

void DrawBoss(Boss *boss) {
    if (!boss->ativo) return;

    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        DrawTexturePro(
            boss->texTiro,
            (Rectangle){0, 0, boss->texTiro.width, boss->texTiro.height},
            (Rectangle){boss->tiros[i].posicao.x, boss->tiros[i].posicao.y, TAMANHO_TIRO_BOSS, TAMANHO_TIRO_BOSS},
            (Vector2){TAMANHO_TIRO_BOSS / 2.0f, TAMANHO_TIRO_BOSS / 2.0f},
            boss->tiros[i].rotacao,
            WHITE
        );
    }

    Texture2D texAtual;

    if (boss->atirando) {
        texAtual = boss->texAtirando;
    } else {
        int frame = ((int)(boss->tempoAnimIdle * 0.9f)) % 2;
        texAtual = frame == 0 ? boss->texIdle1 : boss->texIdle2;
    }

    DrawTexturePro(
        texAtual,
        (Rectangle){0, 0, texAtual.width, texAtual.height},
        (Rectangle){boss->posicao.x, boss->posicao.y, TAMANHO_BOSS, TAMANHO_BOSS},
        (Vector2){TAMANHO_BOSS / 2.0f, TAMANHO_BOSS / 2.0f},
        0,
        WHITE
    );

    int larguraBarra = 300;
    int alturaBarra = 20;
    float vidaPercentual = (float)boss->vidas / boss->vidaMaxima;

    DrawRectangle(boss->posicao.x - 150, boss->posicao.y - 230, larguraBarra, alturaBarra, DARKGRAY);
    DrawRectangle(boss->posicao.x - 150, boss->posicao.y - 230, larguraBarra * vidaPercentual, alturaBarra, RED);
    DrawRectangleLines(boss->posicao.x - 150, boss->posicao.y - 230, larguraBarra, alturaBarra, WHITE);
}

Rectangle GetBossRect(Boss *boss) {
    return (Rectangle){boss->posicao.x - 160, boss->posicao.y - 160, 320, 320};
}

void BossReceberDano(Boss *boss, int dano) {
    boss->vidas -= dano;

    if (boss->vidas <= 0) {
        boss->vidas = 0;
        boss->ativo = false;
    }
}

bool BossFoiDerrotado(Boss *boss) {
    return boss->vidas <= 0;
}

bool CheckBossTirosCollision(Boss *boss, Rectangle playerRect) {
    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        Rectangle tiroRect = {
            boss->tiros[i].posicao.x - 42,
            boss->tiros[i].posicao.y - 42,
            84,
            84
        };

        if (CheckCollisionRecs(tiroRect, playerRect)) {
            boss->tiros[i].ativo = false;
            return true;
        }
    }

    return false;
}

void UnloadBoss(Boss *boss) {
    UnloadTexture(boss->texIdle1);
    UnloadTexture(boss->texIdle2);
    UnloadTexture(boss->texAtirando);
    UnloadTexture(boss->texTiro);
}

