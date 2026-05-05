#include "raylib.h"
#include "objetos.h"
#include <stdio.h>
#include <math.h>

// CONFIGURAÇÕES
#define GRAVIDADE 400
#define FORCA_PULO 450.0f
#define VELOCIDADE_HORIZONTAL 200.0f

// ENUM 
typedef enum {
    IDLE,
    RUN,
    JUMP
} EstadoJogador;

//  ESTRUTURAS
typedef struct {
    Vector2 posicao;
    float velocidadeY;
    bool podePular;
    int vidas;
    EstadoJogador estado;
} Jogador;

typedef struct {
    Rectangle area;
    bool bloqueia;
    Color cor;
} Plataforma;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
    float rotacao;
} Tiro;

// TIROS 
#define MAX_TIROS 50
Tiro tiros[MAX_TIROS];

//  FUNÇÃO DO JOGADOR 
void AtualizarJogador(Jogador *jogador, Plataforma *plataformas, int qtd, float dt) {

    // Movimento horizontal
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  jogador->posicao.x -= VELOCIDADE_HORIZONTAL * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) jogador->posicao.x += VELOCIDADE_HORIZONTAL * dt;

    // Pulo
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && jogador->podePular) {
        jogador->velocidadeY = -FORCA_PULO;
        jogador->podePular = false;
    }

    // Gravidade
    jogador->velocidadeY += GRAVIDADE * dt;

    // Movimento vertical
    jogador->posicao.y += jogador->velocidadeY * dt;

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

            if (jogador->velocidadeY > 0) {

                if (basePlayer >= topoPlataforma &&
                    basePlayer <= topoPlataforma + jogador->velocidadeY * dt) {

                    jogador->posicao.y = p->area.y - 50;
                    jogador->velocidadeY = 0;
                    jogador->podePular = true;
                }
            }
        }
    }

    // Estados
    if (!jogador->podePular) {
        jogador->estado = JUMP;
    }
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        jogador->estado = RUN;
    }
    else {
        jogador->estado = IDLE;
    }
}

//  MAIN 
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

    // Jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 200};
    jogador.vidas = 3;

    // Plataformas
    Plataforma plataformas[] = {
        {{0, 577, LARGURA_TELA, 300}, true, GRAY},
        {{929, 412, 144, 38}, true, RED},
        {{199,356,211,43},true,RED},
        {{189,164,128,41},true,RED},
    };

    int qtdPlataformas = sizeof(plataformas) / sizeof(plataformas[0]);

    // Inimigo
    SpikeBall spike;
    InitSpike(&spike);

    float tempoInvulneravel = 0;

    // Inicializar tiros
    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }

    SetTargetFPS(60);

    //  LOOP 
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
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

        //  DISPARO 
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            Vector2 mouse = GetMousePosition();

            Vector2 direcao = {
                mouse.x - jogador.posicao.x,
                mouse.y - jogador.posicao.y
            };

            float tamanho = sqrt(direcao.x * direcao.x + direcao.y * direcao.y);

            direcao.x /= tamanho;
            direcao.y /= tamanho;

            for (int i = 0; i < MAX_TIROS; i++) {
                if (!tiros[i].ativo) {

                    tiros[i].ativo = true;
                    tiros[i].posicao = jogador.posicao;
                    tiros[i].rotacao = 0;

                    tiros[i].velocidade.x = direcao.x * 300;
                    tiros[i].velocidade.y = direcao.y * 300;

                    break;
                }
            }
        }

        // ATUALIZAR TIROS
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {

                tiros[i].posicao.x += tiros[i].velocidade.x * dt;
                tiros[i].posicao.y += tiros[i].velocidade.y * dt;
                tiros[i].rotacao += 200 * dt;

                if (tiros[i].posicao.x < 0 || tiros[i].posicao.x > LARGURA_TELA ||
                    tiros[i].posicao.y < 0 || tiros[i].posicao.y > ALTURA_TELA) {
                    tiros[i].ativo = false;
                }
            }
        }

        // DESENHO 
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Plataformas
        for (int i = 0; i < qtdPlataformas; i++) {
            DrawRectangleRec(plataformas[i].area, plataformas[i].cor);
        }
        // Fundo
        DrawTexturePro(
            texFundo,
            (Rectangle){0, 0, texFundo.width, texFundo.height},
            (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );


        // Inimigo
        DrawSpike(spike);

        // ESCOLHA DO FRAME DE PULO
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

        // Jogador
        DrawTexturePro(
            texAtual,
            (Rectangle){0, 0, texAtual.width, texAtual.height},
            (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
            (Vector2){50, 50},
            0.0f,
            WHITE
        );

        // TIROS
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                DrawTexturePro(
                    texEstrela,
                    (Rectangle){0, 0, texEstrela.width, texEstrela.height},
                    (Rectangle){tiros[i].posicao.x, tiros[i].posicao.y, 30, 30},
                    (Vector2){15, 15},
                    tiros[i].rotacao,
                    WHITE
                );
            }
        }

        // UI
        DrawText(TextFormat("Vidas: %d", jogador.vidas), 30, 30, 30, RED);

        EndDrawing();
    }

    // FINALIZAÇÃO
    UnloadSpike(&spike);
    UnloadTexture(texFundo);
    UnloadTexture(texPrincesa);
    UnloadTexture(texEstrela);

    for (int i = 0; i < 3; i++) UnloadTexture(jump[i]);

    CloseWindow();
    return 0;
}