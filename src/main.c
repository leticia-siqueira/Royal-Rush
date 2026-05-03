#include "raylib.h"
#include "objetos.h"
#include <stdio.h>

// ================= CONFIGURAÇÕES =================
#define GRAVIDADE 400
#define FORCA_PULO 450.0f
#define VELOCIDADE_HORIZONTAL 200.0f

// ================= ESTRUTURAS =================
typedef struct {
    Vector2 posicao;
    float velocidadeY;
    bool podePular;
    int vidas;
} Jogador;

typedef struct {
    Rectangle area;
    bool bloqueia;
    Color cor;
} Plataforma;

// ================= FUNÇÃO DO JOGADOR =================
void AtualizarJogador(Jogador *jogador, Plataforma *plataformas, int qtd, float dt) {

    // Movimento horizontal
    if (IsKeyDown(KEY_LEFT))  jogador->posicao.x -= VELOCIDADE_HORIZONTAL * dt;
    if (IsKeyDown(KEY_RIGHT)) jogador->posicao.x += VELOCIDADE_HORIZONTAL * dt;

    // Pulo
    if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP)) && jogador->podePular) {
        jogador->velocidadeY = -FORCA_PULO;
        jogador->podePular = false;
    }

    // Gravidade
    jogador->velocidadeY += GRAVIDADE * dt;

    // Movimento vertical
    jogador->posicao.y += jogador->velocidadeY * dt;

    // Reset do pulo
    jogador->podePular = false;

    Rectangle playerRect = {
    jogador->posicao.x - 50,
    jogador->posicao.y - 50,
    100,
    100
    };

    // Colisão com plataformas
    for (int i = 0; i < qtd; i++) {
        Plataforma *p = &plataformas[i];

        float basePlayer = playerRect.y + playerRect.height;
        float topoPlataforma = p->area.y;

        if (p->bloqueia && CheckCollisionRecs(playerRect, p->area)) {

            if(jogador->velocidadeY > 0){

                if (basePlayer >= topoPlataforma && basePlayer <= topoPlataforma + jogador->velocidadeY * dt){

                    jogador->posicao.y = p->area.y - 50;
                    jogador->velocidadeY = 0;
                    jogador->podePular = true;

                }
            }

        }
    }
}

// ================= MAIN =================
int main(void) {

    const int LARGURA_TELA = 1280;
    const int ALTURA_TELA = 720;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bubble Rush");

    // Texturas
    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo = LoadTexture("imagens/fundo.png");

    // Jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 200};

    // Plataformas
    Plataforma plataformas[] = {
        {{0, 577, LARGURA_TELA, 300}, true, GRAY}, // chão
        {{929, 412, 144, 38}, true, RED}           // plataforma
    };

    int qtdPlataformas = sizeof(plataformas) / sizeof(plataformas[0]);

    // Inimigo
    SpikeBall spike;
    InitSpike(&spike);

    // Estado do jogo
    int vidas = 3;
    float tempoInvulneravel = 0;

    SetTargetFPS(60);

    // ================= LOOP =================
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // Debug: pegar posição do mouse
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            printf("X: %.0f Y: %.0f\n", mouse.x, mouse.y);
        }

        // Atualizações
        AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
        UpdateSpike(&spike);

        if (tempoInvulneravel > 0) tempoInvulneravel -= dt;

        // Hitbox do jogador
        Rectangle hitboxJogador = {
            //jogador.posiçao é o centro da imagem
            jogador.posicao.x - 50,
            jogador.posicao.y - 50,
            100,
            100
        };

        // Colisão com inimigo
        if (CheckSpikeCollision(spike, hitboxJogador) && tempoInvulneravel <= 0) {
            vidas--;
            tempoInvulneravel = 1.0f;
            spike.position.x = LARGURA_TELA;
        }

        // ================= DESENHO =================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Fundo
        DrawTexturePro(
            texFundo,
            (Rectangle){0, 0, texFundo.width, texFundo.height},
            (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        // Plataformas
        for (int i = 0; i < qtdPlataformas; i++) {
            DrawRectangleRec(plataformas[i].area, plataformas[i].cor);
        }

        // Inimigo
        DrawSpike(spike);

        // Jogador (princesa)
        DrawTexturePro(
            texPrincesa,
            (Rectangle){0, 0, texPrincesa.width, texPrincesa.height},
            (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
            (Vector2){50, 50},
            0.0f,
            WHITE
        );

        // UI
        DrawText(TextFormat("Vidas: %d", vidas), 30, 30, 30, RED);

        EndDrawing();
    }

    // ================= FINALIZAÇÃO =================
    UnloadSpike(&spike);
    UnloadTexture(texFundo);
    UnloadTexture(texPrincesa);

    CloseWindow();
    return 0;
}