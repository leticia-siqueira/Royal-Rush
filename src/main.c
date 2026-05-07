#include "raylib.h"
#include "player.h"
#include "objetos.h"
#include "tiro.h"
#include <stdio.h>

#define VELOCIDADE_MAPA 200

typedef enum{
    MENU,
    JOGO,
    GAMEOVER
} EstadoJogo;

int main(void) {

    const int LARGURA_TELA = 1280;
    const int ALTURA_TELA = 720;

    EstadoJogo estado = MENU;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Royal Rush");

    // ================= TEXTURAS =================
    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo = LoadTexture("imagens/fundo.png");
    Texture2D texEstrela = LoadTexture("imagens/estrela.png");
    Texture2D texMenu = LoadTexture("imagens/menu.png");
    Texture2D texGameOver = LoadTexture("imagens/GAMEOVER.png");

    // ================= FRAMES DO PULO =================
    Texture2D jump[3];

    jump[0] = LoadTexture("imagens/pulando1.png");
    jump[1] = LoadTexture("imagens/pulando2.png");
    jump[2] = LoadTexture("imagens/pulando3.png");

    // ================= JOGADOR =================
    Jogador jogador = {0};

    jogador.posicao = (Vector2){400, 200};
    jogador.vidas = 3;

    // ================= PLATAFORMAS =================
    Plataforma plataformas[] = {

        // chão fixo
        {{0, 577, LARGURA_TELA, 300}, true, BROWN},

        // plataforma baixa
        {{1000, 480, 260, 40}, true, BROWN},

        // plataforma alta
        {{1500, 250, 180, 40}, true, BROWN},
    };

    int qtdPlataformas = sizeof(plataformas) / sizeof(plataformas[0]);

    // ================= MATRIZ DE SEQUÊNCIAS =================
    // {altura Y, largura}

    int sequencias[4][2] = {

        {480, 260}, // baixa grande
        {250, 180}, // alta média

        {480, 180}, // baixa média
        {250, 260}  // alta grande
    };

    int indiceSequencia = 0;

    // controla onde a próxima plataforma vai nascer
    float proximaPosicaoX = 1700;

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
            AtualizarJogador(
                &jogador,
                plataformas,
                qtdPlataformas,
                dt
            );

            AtualizarTiros(
                dt,
                LARGURA_TELA,
                ALTURA_TELA
            );

            UpdateSpike(&spike);

            // -------- TEMPO DE INVULNERABILIDADE --------
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

                // reseta posição do inimigo
                spike.position.x = LARGURA_TELA;
            }

            // ================= MOVIMENTO DAS PLATAFORMAS =================
            // começa em 1 porque 0 é o chão

            for (int i = 1; i < qtdPlataformas; i++) {

                // move plataformas para esquerda
                plataformas[i].area.x -= VELOCIDADE_MAPA * dt;

                // quando sair totalmente da tela
                if (plataformas[i].area.x +
                    plataformas[i].area.width < 0) {

                    // nasce depois da última
                    plataformas[i].area.x = proximaPosicaoX;

                    // pega altura da matriz
                    plataformas[i].area.y =
                        sequencias[indiceSequencia][0];

                    // pega largura da matriz
                    plataformas[i].area.width =
                        sequencias[indiceSequencia][1];

                    // distância entre plataformas
                    proximaPosicaoX += 400;

                    // próxima sequência
                    indiceSequencia++;

                    // reinicia sequência
                    if (indiceSequencia >= 4) {
                        indiceSequencia = 0;
                    }
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
                (Vector2){0, 0},
                0,
                WHITE
            );
        }

        // ================= JOGO =================
        else if (estado == JOGO) {

            // ================= CHÃO =================

            // corpo do chão
            DrawRectangleRec(plataformas[0].area, BROWN);


            // -------- FUNDO --------
            DrawTexturePro(
                texFundo,
                (Rectangle){0, 0, texFundo.width, texFundo.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );


            // ================= PLATAFORMAS =================
            for (int i = 1; i < qtdPlataformas; i++) {

                Rectangle p = plataformas[i].area;

                // corpo principal
                DrawRectangleRec(p, BROWN);

                // borda clara
                DrawRectangle(
                    p.x,
                    p.y,
                    p.width,
                    6,
                    BEIGE
                );

                // linhas verticais
                for (int x = p.x; x < p.x + p.width; x += 40) {

                    DrawLine(
                        x,
                        p.y,
                        x,
                        p.y + p.height,
                        DARKBROWN
                    );
                }

                // linha horizontal
                DrawLine(
                    p.x,
                    p.y + p.height / 2,
                    p.x + p.width,
                    p.y + p.height / 2,
                    DARKBROWN
                );
            }

            // ================= INIMIGO =================
            DrawSpike(spike);

            // ================= ANIMAÇÃO DO JOGADOR =================
            Texture2D texAtual = texPrincesa;

            if (jogador.estado == JUMP) {

                // subindo
                if (jogador.velocidadeY < -50) {
                    texAtual = jump[0];
                }

                // descendo
                else if (jogador.velocidadeY > 50) {
                    texAtual = jump[2];
                }

                // topo do pulo
                else {
                    texAtual = jump[1];
                }
            }

            // ================= JOGADOR =================
            DrawTexturePro(
                texAtual,
                (Rectangle){0, 0, texAtual.width, texAtual.height},
                (Rectangle){
                    jogador.posicao.x,
                    jogador.posicao.y,
                    100,
                    100
                },
                (Vector2){50, 50},
                0,
                WHITE
            );

            // ================= TIROS =================
            DesenharTiros(texEstrela);

            // ================= UI =================
            DrawText(
                TextFormat("Vidas: %d", jogador.vidas),
                30,
                30,
                30,
                RED
            );
        }

        // ================= GAME OVER =================
        else {

            DrawTexturePro(
                texGameOver,
                (Rectangle){
                    0,
                    0,
                    texGameOver.width,
                    texGameOver.height
                },

                (Rectangle){
                    0,
                    0,
                    LARGURA_TELA,
                    ALTURA_TELA
                },

                (Vector2){0, 0},
                0,
                WHITE
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

    for (int i = 0; i < 3; i++) {
        UnloadTexture(jump[i]);
    }

    CloseWindow();

    return 0;
}