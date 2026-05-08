#include "raylib.h"
#include "player.h"
#include "objetos.h"
#include "tiro.h"
#include <stdio.h>

#define VELOCIDADE_MAPA 200

typedef enum {
    MENU,
    JOGO,
    GAMEOVER
} EstadoJogo;

int main(void) {

    // ================= TELA CHEIA =================
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Royal Rush");

    int LARGURA_TELA = GetScreenWidth();
    int ALTURA_TELA  = GetScreenHeight();

    EstadoJogo estado = MENU;

    // ================= TEXTURAS =================
    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo    = LoadTexture("imagens/fundo.png");
    Texture2D texEstrela  = LoadTexture("imagens/estrela.png");
    Texture2D texMenu     = LoadTexture("imagens/menu.png");
    Texture2D texGameOver = LoadTexture("imagens/GAMEOVER.png");

    // ================= FRAMES DO PULO =================
    Texture2D jump[3];
    jump[0] = LoadTexture("imagens/pulando1.png");
    jump[1] = LoadTexture("imagens/pulando2.png");
    jump[2] = LoadTexture("imagens/pulando3.png");

    // ================= JOGADOR =================
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, ALTURA_TELA * 0.30f};
    jogador.vidas   = 3;

    // ================= PROPORÇÕES VISUAIS =================
    // Baseadas no fundo: tijolos começam em ~80% da altura da tela
    float chaoY      = ALTURA_TELA * 0.80f;
    float platBaixaY = ALTURA_TELA * 0.67f;
    float platAltaY  = ALTURA_TELA * 0.35f;

    // ================= PLATAFORMAS =================
    Plataforma plataformas[3];

    // chão — apenas colisão, não será desenhado
    plataformas[0].area  = (Rectangle){0, chaoY, (float)LARGURA_TELA, (float)ALTURA_TELA};
    plataformas[0].bloqueia = true;
    plataformas[0].cor   = BROWN;

    // plataforma baixa — entra da direita
    plataformas[1].area  = (Rectangle){(float)LARGURA_TELA + 200, platBaixaY, 260, 40};
    plataformas[1].bloqueia = true;
    plataformas[1].cor   = BROWN;

    // plataforma alta — ainda mais à direita
    plataformas[2].area  = (Rectangle){(float)LARGURA_TELA + 700, platAltaY, 180, 40};
    plataformas[2].bloqueia = true;
    plataformas[2].cor   = BROWN;

    int qtdPlataformas = 3;

    // ================= SEQUÊNCIAS DE PLATAFORMAS =================
    // {altura Y, largura} — usando os mesmos Y proporcionais
    float sequencias[4][2] = {
        {platBaixaY, 260},
        {platAltaY,  180},
        {platBaixaY, 180},
        {platAltaY,  260}
    };

    int indiceSequencia = 0;

    // ================= INIMIGO =================
    SpikeBall spike;
    InitSpike(&spike);
    float tempoInvulneravel = 0;

    // ================= TIROS =================
    InitTiros();

    SetTargetFPS(60);

    // ================= LOOP PRINCIPAL =================
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // ================= MENU =================
        if (estado == MENU) {

            if (IsKeyPressed(KEY_ENTER) ||
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                estado = JOGO;
            }
        }

        // ================= JOGO =================
        else if (estado == JOGO) {

            // -------- DISPARO --------
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                DispararTiro(jogador.posicao, mouse);
            }

            // -------- ATUALIZAÇÕES --------
            AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
            AtualizarTiros(dt, LARGURA_TELA, ALTURA_TELA);
            UpdateSpike(&spike);

            // -------- INVULNERABILIDADE --------
            if (tempoInvulneravel > 0) {
                tempoInvulneravel -= dt;
            }

            // -------- HITBOX DO JOGADOR --------
            Rectangle hitboxJogador = {
                jogador.posicao.x - 50,
                jogador.posicao.y - 50,
                100,
                100
            };

            // -------- COLISÃO COM INIMIGO --------
            if (CheckSpikeCollision(spike, hitboxJogador)
                && tempoInvulneravel <= 0) {

                jogador.vidas--;
                tempoInvulneravel = 1.0f;
                spike.position.x = (float)LARGURA_TELA;
            }

            // -------- MOVIMENTO DAS PLATAFORMAS --------
            for (int i = 1; i < qtdPlataformas; i++) {

                plataformas[i].area.x -= VELOCIDADE_MAPA * dt;

                // saiu pela esquerda
                if (plataformas[i].area.x + plataformas[i].area.width < 0) {

                    // maiorX começa em LARGURA_TELA para garantir que
                    // a nova plataforma sempre nasça fora da tela
                    float maiorX = (float)LARGURA_TELA;

                    for (int j = 1; j < qtdPlataformas; j++) {
                        if (j != i && plataformas[j].area.x > maiorX) {
                            maiorX = plataformas[j].area.x;
                        }
                    }

                    plataformas[i].area.x     = maiorX + GetRandomValue(350, 500);
                    plataformas[i].area.y     = sequencias[indiceSequencia][0];
                    plataformas[i].area.width = sequencias[indiceSequencia][1];

                    indiceSequencia++;
                    if (indiceSequencia >= 4) indiceSequencia = 0;
                }
            }

            // -------- GAME OVER --------
            if (jogador.vidas <= 0) {
                estado = GAMEOVER;
            }
        }

        // ================= DESENHO =================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ================= MENU =================
        if (estado == MENU) {

            DrawTexturePro(
                texMenu,
                (Rectangle){0, 0, texMenu.width, texMenu.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0}, 0, WHITE
            );
        }

        // ================= JOGO =================
        else if (estado == JOGO) {

            // 1) FUNDO — desenhado primeiro, cobre tudo
            DrawTexturePro(
                texFundo,
                (Rectangle){0, 0, texFundo.width, texFundo.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0}, 0, WHITE
            );

            // 2) PLATAFORMAS FLUTUANTES — por cima do fundo
            for (int i = 1; i < qtdPlataformas; i++) {

                Rectangle p = plataformas[i].area;

                // corpo principal
                DrawRectangleRec(p, BROWN);

                // borda clara no topo
                DrawRectangle(p.x, p.y, p.width, 6, BEIGE);

                // linhas verticais decorativas
                for (int x = p.x; x < p.x + p.width; x += 40) {
                    DrawLine(x, p.y, x, p.y + p.height, DARKBROWN);
                }

                // linha horizontal central
                DrawLine(
                    p.x, p.y + p.height / 2,
                    p.x + p.width, p.y + p.height / 2,
                    DARKBROWN
                );
            }

            // 3) INIMIGO
            DrawSpike(spike);

            // 4) ANIMAÇÃO DO JOGADOR
            Texture2D texAtual = texPrincesa;

            if (jogador.estado == JUMP) {
                if      (jogador.velocidadeY < -50) texAtual = jump[0]; // subindo
                else if (jogador.velocidadeY >  50) texAtual = jump[2]; // descendo
                else                                texAtual = jump[1]; // topo
            }

            DrawTexturePro(
                texAtual,
                (Rectangle){0, 0, texAtual.width, texAtual.height},
                (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
                (Vector2){50, 50}, 0, WHITE
            );

            // 5) TIROS
            DesenharTiros(texEstrela);

            // 6) UI
            DrawText(
                TextFormat("Vidas: %d", jogador.vidas),
                30, 30, 30, RED
            );
        }

        // ================= GAME OVER =================
        else {

            DrawTexturePro(
                texGameOver,
                (Rectangle){0, 0, texGameOver.width, texGameOver.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0}, 0, WHITE
            );
        }

        EndDrawing();
    }

    // ================= FINALIZAÇÃO =================
    UnloadSpike(&spike);
    UnloadTexture(texFundo);
    UnloadTexture(texPrincesa);
    UnloadTexture(texEstrela);
    UnloadTexture(texMenu);
    UnloadTexture(texGameOver);

    for (int i = 0; i < 3; i++) UnloadTexture(jump[i]);

    CloseWindow();
    return 0;
}