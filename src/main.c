#include "raylib.h"
#include "player.h"
#include "tiro.h"
#include "bruxa.h"
#include "cogumelo.h"
#include <math.h>
#include <stdio.h>

#define VELOCIDADE_MAPA     200
#define META_KILLS          25   // objetivo da fase 1

extern void GetTiroRect(int index, Rectangle *out, bool *ativo);
extern int  GetMaxTiros(void);

typedef enum {
    MENU,
    JOGO,
    GAMEOVER
} EstadoJogo;

int main(void) {

    // ── TELA CHEIA ────────────────────────────────────────────
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Royal Rush");

    int LARGURA_TELA = GetScreenWidth();
    int ALTURA_TELA  = GetScreenHeight();

    EstadoJogo estado = MENU;

    // ── TEXTURAS ──────────────────────────────────────────────
    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo    = LoadTexture("imagens/fundo.png");
    Texture2D texEstrela  = LoadTexture("imagens/estrela.png");
    Texture2D texMenu     = LoadTexture("imagens/menu.png");
    Texture2D texGameOver = LoadTexture("imagens/GAMEOVER.png");

    Texture2D jump[3];
    jump[0] = LoadTexture("imagens/pulando1.png");
    jump[1] = LoadTexture("imagens/pulando2.png");
    jump[2] = LoadTexture("imagens/pulando3.png");

    // ── JOGADOR ───────────────────────────────────────────────
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, ALTURA_TELA * 0.30f};
    jogador.vidas   = 3;

    // ── PROPORÇÕES VISUAIS ────────────────────────────────────
    float chaoY      = ALTURA_TELA * 0.80f;
    float platBaixaY = ALTURA_TELA * 0.67f;
    float platAltaY  = ALTURA_TELA * 0.35f;

    // ── PLATAFORMAS ───────────────────────────────────────────
    Plataforma plataformas[4];

    plataformas[0].area     = (Rectangle){0, chaoY, (float)LARGURA_TELA, (float)ALTURA_TELA};
    plataformas[0].bloqueia = true;
    plataformas[0].cor      = BROWN;

    plataformas[1].area     = (Rectangle){(float)LARGURA_TELA + 200, platBaixaY, 260, 40};
    plataformas[1].bloqueia = true;
    plataformas[1].cor      = BROWN;

    plataformas[2].area     = (Rectangle){(float)LARGURA_TELA + 700, platAltaY, 180, 40};
    plataformas[2].bloqueia = true;
    plataformas[2].cor      = BROWN;

    plataformas[3].area     = (Rectangle){(float)LARGURA_TELA + 1200, platBaixaY, 220, 40};
    plataformas[3].bloqueia = true;
    plataformas[3].cor      = BROWN;

    int qtdPlataformas = 4;

    float sequencias[4][2] = {
        {platBaixaY, 260},
        {platAltaY,  180},
        {platBaixaY, 180},
        {platAltaY,  260}
    };
    int indiceSequencia = 0;

    // ── INIMIGOS ──────────────────────────────────────────────
    Bruxa     bruxa;
    CogumeloRei cogumelo;

    InitBruxa(&bruxa);
    InitCogumelo(&cogumelo, LARGURA_TELA, chaoY);

    float tempoInvulneravel = 0;

    // ── TIROS DO JOGADOR ──────────────────────────────────────
    InitTiros();
    
    float timerTiro = 0;

    // ── CONTADOR DE KILLS ─────────────────────────────────────
    int kills = 0;

    SetTargetFPS(60);

    // ══════════════════════════════════════════════════════════
    // LOOP PRINCIPAL
    // ══════════════════════════════════════════════════════════
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // ── MENU ──────────────────────────────────────────────
        if (estado == MENU) {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                estado = JOGO;
            }
        }
        else if(estado == JOGO){
            
            float dt = GetFrameTime();
            timerTiro -= dt;
            
            // INPUT DE TIRO
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && timerTiro<= 0) {

                int ativos = ContarTirosAtivos();

                if(ativos < 5){
                    Vector2 mouse = GetMousePosition();
                    DispararTiro(jogador.posicao, mouse);
                    timerTiro = 0.2f;
                }
            }

            // Atualizações principais
            AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
            AtualizarTiros(dt, LARGURA_TELA, ALTURA_TELA);
            UpdateBruxa(&bruxa, jogador.posicao, LARGURA_TELA, ALTURA_TELA);
            UpdateCogumelo(&cogumelo, LARGURA_TELA, chaoY);

            // Invulnerabilidade
            if (tempoInvulneravel > 0) tempoInvulneravel -= dt;

            // Hitbox do jogador
            Rectangle hitboxJogador = {
                jogador.posicao.x - 50,
                jogador.posicao.y - 50,
                100, 100
            };

            // ── COLISÃO TIROS DO JOGADOR → BRUXA ──────────────
            if (bruxa.ativa && kills < META_KILLS) {
                Rectangle hitboxBruxa = {
                    bruxa.posicao.x - 75,
                    bruxa.posicao.y - 75,
                    150, 150
                };
                int maxT = GetMaxTiros();
                for (int i = 0; i < maxT; i++) {
                    Rectangle tiroRect;
                    bool tiroAtivo;
                    GetTiroRect(i, &tiroRect, &tiroAtivo);
                    if (tiroAtivo && CheckCollisionRecs(tiroRect, hitboxBruxa)) {

                            // remove 1 vida da bruxa
                            bruxa.vidas--;

                            // desativa o tiro que acertou
                            DesativarTiro(i);

                            // se a vida acabar
                            if (bruxa.vidas <= 0) {

                                bruxa.ativa = false;

                                bruxa.tempoCooldownAparicao = 4.0f;

                                // soma kill apenas quando realmente mata
                                kills++;

                                if (kills > META_KILLS) {
                                    kills = META_KILLS;
                                }
                            }

                            break;
                        }
                }
            }

            // ── COLISÃO TIROS DO JOGADOR → COGUMELO ───────────
            if (cogumelo.ativo && kills < META_KILLS) {
                Rectangle hitboxCogumelo = {
                    cogumelo.posicao.x - 35,
                    cogumelo.posicao.y - 35,
                    70, 70
                };
                int maxT = GetMaxTiros();
                for (int i = 0; i < maxT; i++) {
                    Rectangle tiroRect;
                    bool tiroAtivo;
                    GetTiroRect(i, &tiroRect, &tiroAtivo);
                    if (tiroAtivo && CheckCollisionRecs(tiroRect, hitboxCogumelo)) {

                        cogumelo.vidas--;
                        DesativarTiro(i);

                        if(cogumelo.vidas <= 0){

                            cogumelo.ativo = false;
                            cogumelo.tempoCooldownAparicao = 3.0f;
                            DesativarTiro(i);
                            kills++;
                            if (kills > META_KILLS) kills = META_KILLS;
                        }
                        break;
                    }
                }
            }

            // ── COLISÃO JOGADOR → TIROS DA BRUXA ──────────────
            if (tempoInvulneravel <= 0) {
                for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
                    if (!bruxa.tiros[i].ativo) continue;
                    Rectangle tiroRect = {
                        bruxa.tiros[i].posicao.x - 21,
                        bruxa.tiros[i].posicao.y - 21,
                        42, 42
                    };
                    if (CheckCollisionRecs(tiroRect, hitboxJogador)) {
                        jogador.vidas--;
                        tempoInvulneravel = 1.0f;
                        bruxa.tiros[i].ativo = false;
                        break;
                    }
                }
            }

            // ── COLISÃO JOGADOR → COGUMELO ────────────────────
            if (CheckCogumeloCollision(&cogumelo, hitboxJogador) && tempoInvulneravel <= 0) {
                jogador.vidas--;
                tempoInvulneravel = 1.0f;
                cogumelo.ativo = false;
                cogumelo.tempoCooldownAparicao = 3.0f;
            }

            // ── COLISÃO JOGADOR → BRUXA ─────────────
            if (bruxa.ativa && tempoInvulneravel <= 0) {
                Rectangle hitboxBruxa = {
                    bruxa.posicao.x - 60,
                    bruxa.posicao.y - 60,
                    120, 120
                };
                if (CheckCollisionRecs(hitboxBruxa, hitboxJogador)) {
                    jogador.vidas--;
                    tempoInvulneravel = 1.0f;
                }
            }

            // ── PLATAFORMAS ───────────────────────────────────
            for (int i = 1; i < qtdPlataformas; i++) {
                plataformas[i].area.x -= VELOCIDADE_MAPA * dt;

                if (plataformas[i].area.x + plataformas[i].area.width < 0) {
                    float maiorX = (float)LARGURA_TELA;
                    for (int j = 1; j < qtdPlataformas; j++) {
                        if (j != i && plataformas[j].area.x > maiorX)
                            maiorX = plataformas[j].area.x;
                    }
                    plataformas[i].area.x     = maiorX + GetRandomValue(350, 500);
                    plataformas[i].area.y     = sequencias[indiceSequencia][0];
                    plataformas[i].area.width = sequencias[indiceSequencia][1];
                    indiceSequencia++;
                    if (indiceSequencia >= 4) indiceSequencia = 0;
                }
            }

            // ── GAME OVER ─────────────────────────────────────
            if (jogador.vidas <= 0) estado = GAMEOVER;
        }

        // ══════════════════════════════════════════════════════
        // DESENHO
        // ══════════════════════════════════════════════════════
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // MENU
        if (estado == MENU) {
            DrawTexturePro(texMenu,
                (Rectangle){0,0,texMenu.width,texMenu.height},
                (Rectangle){0,0,LARGURA_TELA,ALTURA_TELA},
                (Vector2){0,0}, 0, WHITE);
        }

        // JOGO
        else if (estado == JOGO) {

            // 1) Fundo
            DrawTexturePro(texFundo,
                (Rectangle){0,0,texFundo.width,texFundo.height},
                (Rectangle){0,0,LARGURA_TELA,ALTURA_TELA},
                (Vector2){0,0}, 0, WHITE);

            // 2) Plataformas estilo tijolo
            for (int i = 1; i < qtdPlataformas; i++) {
                Rectangle p = plataformas[i].area;
                int alturaTijolo = p.height / 2;
                DrawRectangleRec(p, (Color){100, 35, 10, 255});

                for (int linha = 0; linha < 2; linha++) {
                    int deslocamento = (linha % 2 == 0) ? 0 : 20;
                    int y = p.y + linha * alturaTijolo;
                    int x = p.x - 20 + deslocamento;
                    while (x < p.x + p.width) {
                        int tx = (x < p.x) ? p.x : x;
                        int largura = 38;
                        if (tx + largura > p.x + p.width) largura = (p.x + p.width) - tx;
                        if (largura <= 0) { x += 40; continue; }
                        DrawRectangle(tx, y + 1, largura, alturaTijolo - 2, (Color){200, 100, 45, 255});
                        DrawRectangle(tx, y + 1, largura, 4, (Color){220, 130, 70, 255});
                        x += 40;
                    }
                }
            }

            // 3) Inimigos
            DrawBruxa(&bruxa);
            DrawCogumelo(&cogumelo);

            // 4) Jogador
            Texture2D texAtual = texPrincesa;
            if (jogador.estado == JUMP) {
                if      (jogador.velocidadeY < -50) texAtual = jump[0];
                else if (jogador.velocidadeY >  50) texAtual = jump[2];
                else                                texAtual = jump[1];
            }
            DrawTexturePro(texAtual,
                (Rectangle){0,0,texAtual.width,texAtual.height},
                (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
                (Vector2){50, 50}, 0, WHITE);

            // 5) Tiros do jogador
            DesenharTiros(texEstrela);

            // 6) UI — Vidas (canto superior esquerdo)
            DrawText(TextFormat("Vidas: %d", jogador.vidas), 30, 30, 30, RED);

            // 7) UI — Contador de kills (canto superior direito)
            const char *textoKills = TextFormat("%d/%d", kills, META_KILLS);
            int larguraTexto = MeasureText(textoKills, 30);
            DrawText(textoKills, LARGURA_TELA - larguraTexto - 30, 30, 30, GOLD);
        }

        // GAME OVER
        else {
            DrawTexturePro(texGameOver,
                (Rectangle){0,0,texGameOver.width,texGameOver.height},
                (Rectangle){0,0,LARGURA_TELA,ALTURA_TELA},
                (Vector2){0,0}, 0, WHITE);
        }

        EndDrawing();
    }

    // ── FINALIZAÇÃO ───────────────────────────────────────────
    UnloadBruxa(&bruxa);
    UnloadCogumelo(&cogumelo);
    UnloadTexture(texFundo);
    UnloadTexture(texPrincesa);
    UnloadTexture(texEstrela);
    UnloadTexture(texMenu);
    UnloadTexture(texGameOver);
    for (int i = 0; i < 3; i++) UnloadTexture(jump[i]);

    CloseWindow();
    return 0;
}
