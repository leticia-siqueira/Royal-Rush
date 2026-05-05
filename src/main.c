#include "raylib.h"
#include "player.h"
#include "objetos.h"
#include "tiro.h"
#include <stdio.h>

// MAIN 
int main(void) {

    const int LARGURA_TELA = 1280;
    const int ALTURA_TELA = 720;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bubble Rush");

    // TEXTURAS
    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo = LoadTexture("imagens/fundo.png");
    Texture2D texEstrela = LoadTexture("imagens/estrela.png");

    // FRAMES DO PULO
    Texture2D jump[3];
    jump[0] = LoadTexture("imagens/pulando1.png");
    jump[1] = LoadTexture("imagens/pulando2.png");
    jump[2] = LoadTexture("imagens/pulando3.png");

    // JOGADOR
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 200};
    jogador.vidas = 3;

    // PLATAFORMAS
    Plataforma plataformas[] = {
        {{0, 577, LARGURA_TELA, 300}, true, GRAY},
        {{929, 412, 144, 38}, true, RED},
        {{199,356,211,43},true,RED},
        {{189,164,128,41},true,RED},
    };

    int qtdPlataformas = sizeof(plataformas) / sizeof(plataformas[0]);

    // INIMIGO
    SpikeBall spike;
    InitSpike(&spike);

    float tempoInvulneravel = 0;

    // INICIALIZAR TIROS
    InitTiros();

    SetTargetFPS(60);


    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // INPUT DE TIRO
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            DispararTiro(jogador.posicao, mouse);
        }

        //ATUALIZAÇÕES
        AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
        AtualizarTiros(dt, LARGURA_TELA, ALTURA_TELA);
        UpdateSpike(&spike);

        if (tempoInvulneravel > 0) tempoInvulneravel -= dt;

        Rectangle hitboxJogador = {
            jogador.posicao.x - 50,
            jogador.posicao.y - 50,
            100,
            100
        };

        if (CheckSpikeCollision(spike, hitboxJogador) && tempoInvulneravel <= 0) {
            jogador.vidas--;
            tempoInvulneravel = 1.0f;
            spike.position.x = LARGURA_TELA;
        }

        //DESENHO
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // FUNDO
        DrawTexturePro(
            texFundo,
            (Rectangle){0, 0, texFundo.width, texFundo.height},
            (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        // PLATAFORMAS
        for (int i = 0; i < qtdPlataformas; i++) {
            DrawRectangleRec(plataformas[i].area, plataformas[i].cor);
        }

        // INIMIGO
        DrawSpike(spike);

        //ESCOLHA DO FRAME DE PULO
        Texture2D texAtual = texPrincesa;

        if (jogador.estado == JUMP) {

            if (jogador.velocidadeY < -50) {
                texAtual = jump[0]; // subindo
            }
            else if (jogador.velocidadeY > 50) {
                texAtual = jump[2]; // descendo
            }
            else {
                texAtual = jump[1]; // topo
            }
        }

        // JOGADOR
        DrawTexturePro(
            texAtual,
            (Rectangle){0, 0, texAtual.width, texAtual.height},
            (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
            (Vector2){50, 50},
            0.0f,
            WHITE
        );

        //TIROS
        DesenharTiros(texEstrela);

        // UI
        DrawText(TextFormat("Vidas: %d", jogador.vidas), 30, 30, 30, RED);

        EndDrawing();
    }

    //FINALIZAÇÃO
    UnloadSpike(&spike);
    UnloadTexture(texFundo);
    UnloadTexture(texPrincesa);
    UnloadTexture(texEstrela);

    for (int i = 0; i < 3; i++) UnloadTexture(jump[i]);

    CloseWindow();
    return 0;
}