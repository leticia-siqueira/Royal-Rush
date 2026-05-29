#include "bruxa.h"

#include <math.h>

#define INTERVALO_APARICAO_BRUXA 2.7f
#define INTERVALO_TIRO_BRUXA 1.8f
#define VELOCIDADE_TIRO_BRUXA 280.0f
#define VELOCIDADE_TIRO_ROTACAO 250.0f

/* FUNÇÕES AUXILIARES*/

static Vector2 NormalizarVetor(Vector2 vetor) {
    float tamanho = sqrtf(vetor.x * vetor.x + vetor.y * vetor.y);

    if (tamanho == 0) {
        return (Vector2){1, 0};
    }

    return (Vector2){
        vetor.x / tamanho,
        vetor.y / tamanho
    };
}

/* INICIALIZAÇÃO */

void InitBruxa(Bruxa *bruxa) {
    bruxa->ativa = false;
    bruxa->atirando = false;

    bruxa->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
    bruxa->tempoAtirando = 0.0f;
    bruxa->tempoAnimAtaque = 0.0f;

    bruxa->tirosDados = 0;
    bruxa->vidas = 3;

    bruxa->posicao = (Vector2){-200, 0};
    bruxa->velocidade = (Vector2){-120, 0};

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        bruxa->tiros[i].ativo = false;
    }

    bruxa->texVoando = LoadTexture("imagens/bruxa_voando.png");
    bruxa->texAtirando = LoadTexture("imagens/bruxa_atirando.png");
    bruxa->texTiro = LoadTexture("imagens/tiro_bruxa.png");
}

static float EscolherAlturaSeguraBruxa(int alturaTela) {
    static int indiceAltura = 0;

    float alturasSeguras[4] = {
        alturaTela * 0.18f,
        alturaTela * 0.53f,
        alturaTela * 0.23f,
        alturaTela * 0.56f
    };

    float alturaEscolhida = alturasSeguras[indiceAltura];

    indiceAltura++;

    if (indiceAltura >= 4) {
        indiceAltura = 0;
    }

    return alturaEscolhida;
}

// UPDATE

void UpdateBruxa(Bruxa *bruxa, Vector2 posJogador, int larguraTela, int alturaTela) {
    float dt = GetFrameTime();

    /* APARIÇÃO DA BRUXA*/

    if (!bruxa->ativa) {
        
        bruxa->tempoCooldownAparicao -= dt;

        if (bruxa->tempoCooldownAparicao <= 0) {
            float yAereo = EscolherAlturaSeguraBruxa(alturaTela);

            bruxa->posicao = (Vector2){(float)larguraTela + 60, yAereo};
            bruxa->velocidade = (Vector2){-170, 0};

            bruxa->ativa = true;
            bruxa->atirando = false;

            bruxa->tirosDados = 0;
            bruxa->tempoAtirando = 0.0f;
            bruxa->tempoAnimAtaque = 0.0f;

            bruxa->vidas = 3;
        }
    }

    /* MOVIMENTO E DISPARO */

    if (bruxa->ativa) {
        if (bruxa->atirando) {
            bruxa->tempoAnimAtaque += dt;

            if (bruxa->tempoAnimAtaque >= 0.4f) {
                bruxa->atirando = false;
                bruxa->tempoAnimAtaque = 0.0f;
            }
        }

        bruxa->posicao.x += bruxa->velocidade.x * dt;

        if (bruxa->posicao.x + 120 < 0) {
            bruxa->ativa = false;
            bruxa->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
        }

        if (bruxa->tirosDados < MAX_TIROS_BRUXA) {
            bruxa->tempoAtirando += dt;

            if (bruxa->tempoAtirando >= INTERVALO_TIRO_BRUXA) {
                bruxa->tempoAtirando = 0.0f;

                for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
                    if (!bruxa->tiros[i].ativo) {
                        Vector2 direcao = {
                            posJogador.x - bruxa->posicao.x,
                            posJogador.y - bruxa->posicao.y
                        };

                        direcao = NormalizarVetor(direcao);

                        bruxa->tiros[i].ativo = true;
                        bruxa->tiros[i].posicao = bruxa->posicao;

                        bruxa->tiros[i].velocidade = (Vector2){
                            direcao.x * VELOCIDADE_TIRO_BRUXA,
                            direcao.y * VELOCIDADE_TIRO_BRUXA
                        };

                        bruxa->tiros[i].rotacao = 0.0f;

                        bruxa->tirosDados++;
                        bruxa->atirando = true;

                        break;
                    }
                }
            }
        }
    }

    /* ATUALIZAÇÃO DOS TIROS DA BRUXA */

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!bruxa->tiros[i].ativo) {
            continue;
        }

        bruxa->tiros[i].posicao.x += bruxa->tiros[i].velocidade.x * dt;
        bruxa->tiros[i].posicao.y += bruxa->tiros[i].velocidade.y * dt;
        bruxa->tiros[i].rotacao += VELOCIDADE_TIRO_ROTACAO * dt;

        if (bruxa->tiros[i].posicao.x < -40 ||
            bruxa->tiros[i].posicao.x > larguraTela + 40 ||
            bruxa->tiros[i].posicao.y < -40 ||
            bruxa->tiros[i].posicao.y > alturaTela + 40) {

            bruxa->tiros[i].ativo = false;
        }
    }
}

/* DRAW */

void DrawBruxa(Bruxa *bruxa) {

    /* DESENHA OS TIROS */

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!bruxa->tiros[i].ativo) {
            continue;
        }

        Texture2D texturaTiro = bruxa->texTiro;

        DrawTexturePro(
            texturaTiro,
            (Rectangle){0, 0, texturaTiro.width, texturaTiro.height},
            (Rectangle){bruxa->tiros[i].posicao.x, bruxa->tiros[i].posicao.y, 30, 30},
            (Vector2){15, 15},
            bruxa->tiros[i].rotacao,
            WHITE
        );
    }

    if (!bruxa->ativa) {
        return;
    }

    /* DESENHA A BRUXA */

    Texture2D texturaAtual = bruxa->atirando ? bruxa->texAtirando : bruxa->texVoando;

    DrawTexturePro(
        texturaAtual,
        (Rectangle){0, 0, texturaAtual.width, texturaAtual.height},
        (Rectangle){bruxa->posicao.x, bruxa->posicao.y, 120, 120},
        (Vector2){60, 60},
        0.0f,
        WHITE
    );

    /* BARRA DE VIDA */

    DrawRectangle(
        bruxa->posicao.x - 30,
        bruxa->posicao.y - 85,
        60,
        8,
        DARKGRAY
    );

    DrawRectangle(
        bruxa->posicao.x - 30,
        bruxa->posicao.y - 85,
        20 * bruxa->vidas,
        8,
        RED
    );
}

/* LIBERAÇÃO DE RECURSOS */

void UnloadBruxa(Bruxa *bruxa) {
    UnloadTexture(bruxa->texVoando);
    UnloadTexture(bruxa->texAtirando);
    UnloadTexture(bruxa->texTiro);
}
