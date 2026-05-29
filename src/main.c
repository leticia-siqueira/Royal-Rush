#include "raylib.h"
#include "player.h"
#include "tiro.h"
#include "bruxa.h"
#include "cogumelo.h"
#include "ranking.h"
#include "objetos.h"
#include "boss.h"

#include <stdio.h>

#define VELOCIDADE_MAPA 200
#define META_KILLS 10

#define PONTOS_BRUXA 150
#define PONTOS_COGUMELO 100
#define PONTOS_BOSS 1000
#define PONTOS_DANO_BOSS 25
#define PENALIDADE_DANO 50

#define TEMPO_TRANSICAO_BOSS 4.5f
#define TEMPO_SPAWN_BOSS 2.0f

typedef enum {
    MENU,
    NOME,
    OBJETIVO,
    JOGO,
    GAMEOVER,
    YOUWIN,
    RANKING
} EstadoJogo;

// FUNÇÕES AUXILIARES DA TELA DE RANKING 

static Rectangle GetBotaoRanking(int larguraTela, int alturaTela) {
    return (Rectangle){
        larguraTela - 320,
        alturaTela - 90,
        280,
        60
    };
}

static void DesenharBotaoRanking(int larguraTela, int alturaTela) {
    Rectangle botaoRanking = GetBotaoRanking(larguraTela, alturaTela);

    DrawRectangleRounded(botaoRanking, 0.25f, 10, (Color){255, 80, 160, 255});
    DrawRectangleRoundedLines(botaoRanking, 0.25f, 10, GOLD);

    const char *texto = "RANKING";
    int larguraTexto = MeasureText(texto, 34);

    DrawText(
        texto,
        botaoRanking.x + botaoRanking.width / 2 - larguraTexto / 2,
        botaoRanking.y + 13,
        34,
        WHITE
    );
}

static void DesenharTelaRanking(Texture2D fundo, int larguraTela, int alturaTela) {
    DrawTexturePro(
        fundo,
        (Rectangle){0, 0, fundo.width, fundo.height},
        (Rectangle){0, 0, larguraTela, alturaTela},
        (Vector2){0, 0},
        0,
        WHITE
    );

    Rectangle painel = {
        larguraTela / 2 - 400,
        alturaTela / 2 - 270,
        800,
        540
    };

    DrawRectangleRec(painel, (Color){255, 230, 220, 240});
    DrawRectangleLinesEx(painel, 5, GOLD);

    const char *titulo = "RANKING";
    int larguraTitulo = MeasureText(titulo, 58);

    DrawText(
        titulo,
        larguraTela / 2 - larguraTitulo / 2,
        alturaTela / 2 - 235,
        58,
        (Color){255, 70, 170, 255}
    );

    FILE *arquivo = fopen("ranking.txt", "r");

    int y = alturaTela / 2 - 145;
    int posicao = 1;

    if (arquivo == NULL) {
        DrawText(
            "Nenhum ranking encontrado ainda.",
            larguraTela / 2 - 240,
            y,
            28,
            DARKPURPLE
        );
    } else {
        char linha[120];

        while (fgets(linha, sizeof(linha), arquivo) != NULL && posicao <= 10) {
            DrawText(
                TextFormat("%d. %s", posicao, linha),
                larguraTela / 2 - 310,
                y,
                26,
                DARKPURPLE
            );

            y += 38;
            posicao++;
        }

        fclose(arquivo);

        if (posicao == 1) {
            DrawText(
                "Ranking vazio.",
                larguraTela / 2 - 90,
                y,
                28,
                DARKPURPLE
            );
        }
    }

    DrawText(
        "Pressione V para voltar",
        larguraTela / 2 - 185,
        alturaTela / 2 + 220,
        24,
        (Color){255, 70, 170, 255}
    );
}

// MAIN 

int main(void) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Royal Rush");
    InitAudioDevice();

    int LARGURA_TELA = GetScreenWidth();
    int ALTURA_TELA = GetScreenHeight();

    EstadoJogo estado = MENU;

    // ÁUDIO 

    Music musica = LoadMusicStream("musicas/gameplay.ogg");
    Music musicaBoss = LoadMusicStream("musicas/boss.ogg");

    PlayMusicStream(musica);
    SetMusicVolume(musica, 0.35f);
    SetMusicVolume(musicaBoss, 0.35f);

    // TEXTURAS 

    Texture2D texPrincesa = LoadTexture("imagens/princesa.png");
    Texture2D texFundo = LoadTexture("imagens/fundo.png");
    Texture2D texFundoBoss = LoadTexture("imagens/fundo_boss.png");
    Texture2D texEstrela = LoadTexture("imagens/estrela.png");
    Texture2D texMenu = LoadTexture("imagens/menu.png");
    Texture2D texObjetivo = LoadTexture("imagens/objetivo.png");
    Texture2D texGameOver = LoadTexture("imagens/GAMEOVER.png");
    Texture2D texYouWin = LoadTexture("imagens/YOUWIN.png");

    Texture2D jump[3];

    jump[0] = LoadTexture("imagens/pulando1.png");
    jump[1] = LoadTexture("imagens/pulando2.png");
    jump[2] = LoadTexture("imagens/pulando3.png");

    // JOGADOR 

    
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, ALTURA_TELA * 0.30f};
    jogador.vidas = 4;
    jogador.velocidadeY = 0;
    jogador.podePular = false;

    // PLATAFORMAS 

    float chaoY = ALTURA_TELA * 0.80f;
    float platBaixaY = ALTURA_TELA * 0.67f;
    float platAltaY = ALTURA_TELA * 0.35f;

    Plataforma plataformas[4];

    plataformas[0].area = (Rectangle){0, chaoY, (float)LARGURA_TELA, (float)ALTURA_TELA};
    plataformas[0].bloqueia = true;
    plataformas[0].cor = BROWN;

    plataformas[1].area = (Rectangle){(float)LARGURA_TELA + 200, platBaixaY, 260, 40};
    plataformas[1].bloqueia = true;
    plataformas[1].cor = BROWN;

    plataformas[2].area = (Rectangle){(float)LARGURA_TELA + 700, platAltaY, 180, 40};
    plataformas[2].bloqueia = true;
    plataformas[2].cor = BROWN;

    plataformas[3].area = (Rectangle){(float)LARGURA_TELA + 1200, platBaixaY, 220, 40};
    plataformas[3].bloqueia = true;
    plataformas[3].cor = BROWN;

    int qtdPlataformas = 4;

    float sequencias[4][2] = {
        {platBaixaY, 260},
        {platAltaY, 180},
        {platBaixaY, 180},
        {platAltaY, 260}
    };
    int indiceSequencia = 0;

    // INIMIGOS E OBJETOS 

    Bruxa bruxa;
    CogumeloRei cogumelo;
    SpikeBall spike;
    Boss boss;

    InitBruxa(&bruxa);
    InitCogumelo(&cogumelo, LARGURA_TELA, chaoY);
    InitSpike(&spike);

    bool modoBoss = false;
    bool bossCriado = false;

    bool transicaoBoss = false;
    float tempoTransicaoBoss = 0.0f;
    bool musicaBossAtiva = false;

    // CONTROLE DE JOGO 

    float tempoInvulneravel = 0;
    float timerTiro = 0;

    int kills = 0;
    int score = 0;

    char nomeJogador[30] = "";
    int letrasNome = 0;

    bool rankingSalvo = false;

    InitTiros();

    SetTargetFPS(60);

    // LOOP PRINCIPAL 

    while (!WindowShouldClose()) {
        if (!musicaBossAtiva) {
            UpdateMusicStream(musica);
        } else {
            UpdateMusicStream(musicaBoss);
        }

        float dt = GetFrameTime();

        // UPDATE 

        if (estado == MENU) {
            Rectangle botaoPlay = {
                LARGURA_TELA / 2 - 130,
                ALTURA_TELA - 190,
                260,
                130
            };

            Rectangle botaoRanking = GetBotaoRanking(LARGURA_TELA, ALTURA_TELA);
            Vector2 mouse = GetMousePosition();

            if (IsKeyPressed(KEY_ENTER)) {
                estado = NOME;
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, botaoRanking)) {
                    estado = RANKING;
                } else if (CheckCollisionPointRec(mouse, botaoPlay)) {
                    estado = NOME;
                }
            }
        }

        else if (estado == RANKING) {
            if (IsKeyPressed(KEY_V)) {
                estado = MENU;
            }
        }

        else if (estado == NOME) {
            int tecla = GetCharPressed();

            while (tecla > 0) {
                if (letrasNome < 29 && tecla >= 32 && tecla <= 125) {
                    nomeJogador[letrasNome] = (char)tecla;
                    letrasNome++;
                    nomeJogador[letrasNome] = '\0';
                }

                tecla = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && letrasNome > 0) {
                letrasNome--;
                nomeJogador[letrasNome] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && letrasNome > 0) {
                estado = OBJETIVO;
            }
        }

        else if (estado == OBJETIVO) {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                estado = JOGO;
            }
        }

        else if (estado == JOGO) {
            timerTiro -= dt;

            // INÍCIO DA TRANSIÇÃO DO BOSS 

            if (kills >= META_KILLS && !modoBoss && !transicaoBoss) {
                transicaoBoss = true;
                tempoTransicaoBoss = 0.0f;

                jogador.vidas = 4;
                bruxa.ativa = false;
                cogumelo.ativo = false;

                StopMusicStream(musica);
                PlayMusicStream(musicaBoss);
                musicaBossAtiva = true;
            }

            if (transicaoBoss) {
                tempoTransicaoBoss += dt;

                if (tempoTransicaoBoss >= TEMPO_SPAWN_BOSS && !bossCriado) {
                    InitBoss(&boss, LARGURA_TELA, ALTURA_TELA);

                    boss.posicao.x = LARGURA_TELA + 150;
                    bossCriado = true;

                    spike.active = true;
                    spike.position.x = LARGURA_TELA + 250;
                    spike.position.y = GetRandomValue(
                        (int)(ALTURA_TELA * 0.25f),
                        (int)(ALTURA_TELA * 0.75f)
                    );
                }

                if (bossCriado && boss.posicao.x > LARGURA_TELA * 0.90f) {
                    boss.posicao.x -= 120 * dt;
                }

                if (tempoTransicaoBoss >= TEMPO_TRANSICAO_BOSS) {
                    transicaoBoss = false;
                    modoBoss = true;
                }
            }

            //TIROS DO JOGADOR 

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && timerTiro <= 0) {
                if (ContarTirosAtivos() < 5) {
                    Vector2 mouse = GetMousePosition();

                    DispararTiro(jogador.posicao, mouse);

                    timerTiro = 0.2f;
                }
            }

            AtualizarJogador(&jogador, plataformas, qtdPlataformas, dt);
            AtualizarTiros(dt, LARGURA_TELA, ALTURA_TELA);

            //UPDATE DOS INIMIGOS 

            if (!modoBoss && !transicaoBoss) {

                UpdateBruxa(&bruxa, jogador.posicao, LARGURA_TELA, ALTURA_TELA);
                UpdateCogumelo(&cogumelo, LARGURA_TELA, chaoY);

            } else if (modoBoss) {

                UpdateBoss(&boss, jogador.posicao, LARGURA_TELA, ALTURA_TELA);
                UpdateSpikeAleatorio(&spike, LARGURA_TELA, ALTURA_TELA);

            }

            if (tempoInvulneravel > 0) {
                tempoInvulneravel -= dt;
            }

            Rectangle hitboxJogador = {
                jogador.posicao.x - 50,
                jogador.posicao.y - 50,
                100,
                100
            };

            //COLISÕES DOS TIROS COM INIMIGOS 

            if (!modoBoss && !transicaoBoss) {
                if (bruxa.ativa && kills < META_KILLS) {
                    Rectangle hitboxBruxa = {
                        bruxa.posicao.x - 75,
                        bruxa.posicao.y - 75,
                        150,
                        150
                    };

                    for (int i = 0; i < GetMaxTiros(); i++) {
                        Rectangle tiroRect;
                        bool tiroAtivo;

                        GetTiroRect(i, &tiroRect, &tiroAtivo);

                        if (tiroAtivo && CheckCollisionRecs(tiroRect, hitboxBruxa)) {
                            bruxa.vidas--;
                            DesativarTiro(i);

                            if (bruxa.vidas <= 0) {
                                bruxa.ativa = false;
                                bruxa.tempoCooldownAparicao = 4.0f;

                                kills++;
                                score += PONTOS_BRUXA;

                                if (kills > META_KILLS) {
                                    kills = META_KILLS;
                                }
                            }

                            break;
                        }
                    }
                }

                if (cogumelo.ativo && kills < META_KILLS) {
                    Rectangle hitboxCogumelo = {
                        cogumelo.posicao.x - 35,
                        cogumelo.posicao.y - 35,
                        70,
                        70
                    };

                    for (int i = 0; i < GetMaxTiros(); i++) {
                        Rectangle tiroRect;
                        bool tiroAtivo;

                        GetTiroRect(i, &tiroRect, &tiroAtivo);

                        if (tiroAtivo && CheckCollisionRecs(tiroRect, hitboxCogumelo)) {
                            cogumelo.vidas--;
                            DesativarTiro(i);

                            if (cogumelo.vidas <= 0) {
                                cogumelo.ativo = false;
                                cogumelo.tempoCooldownAparicao = 3.0f;

                                kills++;
                                score += PONTOS_COGUMELO;

                                if (kills > META_KILLS) {
                                    kills = META_KILLS;
                                }
                            }

                            break;
                        }
                    }
                }
            } 
            else {

                if (bossCriado && boss.ativo) {
                    Rectangle bossRect = GetBossRect(&boss);

                    for (int i = 0; i < GetMaxTiros(); i++) {
                        Rectangle tiroRect;
                        bool tiroAtivo;

                        GetTiroRect(i, &tiroRect, &tiroAtivo);

                        if (tiroAtivo && CheckCollisionRecs(tiroRect, bossRect)) {
                            BossReceberDano(&boss, 1);
                            DesativarTiro(i);

                            score += PONTOS_DANO_BOSS;

                            break;
                        }
                    }

                    if (!transicaoBoss && BossFoiDerrotado(&boss)) {
                        score += PONTOS_BOSS;

                        if (!rankingSalvo) {
                            SalvarRanking(nomeJogador, score);
                            rankingSalvo = true;
                        }

                        estado = YOUWIN;
                    }
                }
            }

            //DANO NO JOGADOR 

            if (tempoInvulneravel <= 0) {
                if (!modoBoss) {
                    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
                        if (!bruxa.tiros[i].ativo) {
                            continue;
                        }

                        Rectangle tiroRect = {
                            bruxa.tiros[i].posicao.x - 21,
                            bruxa.tiros[i].posicao.y - 21,
                            42,
                            42
                        };

                        if (CheckCollisionRecs(tiroRect, hitboxJogador)) {
                            jogador.vidas--;
                            score -= PENALIDADE_DANO;

                            if (score < 0) {
                                score = 0;
                            }

                            tempoInvulneravel = 1.0f;
                            bruxa.tiros[i].ativo = false;

                            break;
                        }
                    }

                    if (CheckCogumeloCollision(&cogumelo, hitboxJogador)) {
                        jogador.vidas--;
                        score -= PENALIDADE_DANO;

                        if (score < 0) {
                            score = 0;
                        }

                        tempoInvulneravel = 1.0f;
                        cogumelo.ativo = false;
                        cogumelo.tempoCooldownAparicao = 3.0f;
                    }

                    if (bruxa.ativa) {
                        Rectangle hitboxBruxaDano = {
                            bruxa.posicao.x - 60,
                            bruxa.posicao.y - 60,
                            120,
                            120
                        };

                        if (CheckCollisionRecs(hitboxBruxaDano, hitboxJogador)) {
                            jogador.vidas--;
                            score -= PENALIDADE_DANO;

                            if (score < 0) {
                                score = 0;
                            }

                            tempoInvulneravel = 1.0f;
                        }
                    }
                } 
                else if (!transicaoBoss) {
                    if (CheckBossTirosCollision(&boss, hitboxJogador)) {
                        jogador.vidas--;
                        score -= PENALIDADE_DANO;

                        if (score < 0) {
                            score = 0;
                        }

                        tempoInvulneravel = 1.0f;
                    }

                    if (CheckSpikeCollision(spike, hitboxJogador)) {
                        jogador.vidas--;
                        score -= PENALIDADE_DANO;

                        if (score < 0) {
                            score = 0;
                        }

                        tempoInvulneravel = 1.0f;

                        spike.position.x = LARGURA_TELA + GetRandomValue(150, 450);
                        spike.position.y = GetRandomValue(
                            (int)(ALTURA_TELA * 0.25f),
                            (int)(ALTURA_TELA * 0.75f)
                        );
                    }
                }
            }

            // MOVIMENTO DAS PLATAFORMAS

            for (int i = 1; i < qtdPlataformas; i++) {
                plataformas[i].area.x -= VELOCIDADE_MAPA * dt;

                if (plataformas[i].area.x + plataformas[i].area.width < 0) {
                    float maiorX = (float)LARGURA_TELA;

                    for (int j = 1; j < qtdPlataformas; j++) {
                        if (j != i && plataformas[j].area.x > maiorX) {
                            maiorX = plataformas[j].area.x;
                        }
                    }

                    plataformas[i].area.x = maiorX + GetRandomValue(350, 500);
                    plataformas[i].area.y = sequencias[indiceSequencia][0];
                    plataformas[i].area.width = sequencias[indiceSequencia][1];

                    indiceSequencia++;

                    if (indiceSequencia >= 4) {
                        indiceSequencia = 0;
                    }
                }
            }

            // FIM DE JOGO

            if (jogador.vidas <= 0) {
                if (!rankingSalvo) {
                    SalvarRanking(nomeJogador, score);
                    rankingSalvo = true;
                }

                estado = GAMEOVER;
            }
        }

        else if (estado == GAMEOVER || estado == YOUWIN) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)) {
                jogador.posicao = (Vector2){400, ALTURA_TELA * 0.30f};
                jogador.vidas = 3;
                jogador.velocidadeY = 0;
                jogador.podePular = false;

                if (bossCriado) {
                    UnloadBoss(&boss);
                    bossCriado = false;
                }

                InitBruxa(&bruxa);
                InitCogumelo(&cogumelo, LARGURA_TELA, chaoY);
                InitTiros();

                modoBoss = false;
                transicaoBoss = false;
                tempoTransicaoBoss = 0.0f;
                spike.active = false;

                if (musicaBossAtiva) {
                    StopMusicStream(musicaBoss);
                    PlayMusicStream(musica);
                    musicaBossAtiva = false;
                }

                kills = 0;
                score = 0;
                rankingSalvo = false;

                nomeJogador[0] = '\0';
                letrasNome = 0;

                plataformas[1].area.x = (float)LARGURA_TELA + 200;
                plataformas[2].area.x = (float)LARGURA_TELA + 700;
                plataformas[3].area.x = (float)LARGURA_TELA + 1200;

                indiceSequencia = 0;

                estado = MENU;
            }
        }

        // DESENHO

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (estado == MENU) {
            DrawTexturePro(
                texMenu,
                (Rectangle){0, 0, texMenu.width, texMenu.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );

            DesenharBotaoRanking(LARGURA_TELA, ALTURA_TELA);
        }

        else if (estado == RANKING) {
            DesenharTelaRanking(texFundo, LARGURA_TELA, ALTURA_TELA);
        }

        else if (estado == NOME) {
            DrawTexturePro(
                texMenu,
                (Rectangle){0, 0, texMenu.width, texMenu.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );

            DrawRectangle(
                LARGURA_TELA / 2 - 300,
                ALTURA_TELA / 2 - 110,
                600,
                220,
                (Color){0, 0, 0, 190}
            );

            DrawText(
                "Digite seu nome:",
                LARGURA_TELA / 2 - 170,
                ALTURA_TELA / 2 - 70,
                32,
                WHITE
            );

            DrawRectangleLines(
                LARGURA_TELA / 2 - 180,
                ALTURA_TELA / 2 - 20,
                360,
                50,
                GOLD
            );

            DrawText(
                nomeJogador,
                LARGURA_TELA / 2 - 165,
                ALTURA_TELA / 2 - 8,
                28,
                GOLD
            );

            DrawText(
                "Pressione ENTER para continuar",
                LARGURA_TELA / 2 - 210,
                ALTURA_TELA / 2 + 60,
                22,
                WHITE
            );
        }

        else if (estado == OBJETIVO) {
            DrawTexturePro(
                texObjetivo,
                (Rectangle){0, 0, texObjetivo.width, texObjetivo.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );
        }

        else if (estado == JOGO) {
            Texture2D fundoAtual = (modoBoss || transicaoBoss) ? texFundoBoss : texFundo;

            DrawTexturePro(
                fundoAtual,
                (Rectangle){0, 0, fundoAtual.width, fundoAtual.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );

            // PLATAFORMAS

            for (int i = 1; i < qtdPlataformas; i++) {

                Rectangle plataformaAtual = plataformas[i].area;
                int alturaTijolo = plataformaAtual.height / 2;

                DrawRectangleRec(plataformaAtual, (Color){100, 35, 10, 255});

                for (int linha = 0; linha < 2; linha++) {

                    int deslocamento = (linha % 2 == 0) ? 0 : 20;

                    int y = plataformaAtual.y + linha * alturaTijolo;
                    int x = plataformaAtual.x - 20 + deslocamento;

                    while (x < plataformaAtual.x + plataformaAtual.width) {

                        int xTijolo = (x < plataformaAtual.x) ? plataformaAtual.x : x;
                        int larguraTijolo = 38;

                        if (xTijolo + larguraTijolo > plataformaAtual.x + plataformaAtual.width) {
                            larguraTijolo = (plataformaAtual.x + plataformaAtual.width) - xTijolo;
                        }

                        if (larguraTijolo <= 0) {
                            x += 40;
                            continue;
                        }

                        DrawRectangle(
                            xTijolo,
                            y + 1,
                            larguraTijolo,
                            alturaTijolo - 2,
                            (Color){200, 100, 45, 255}
                        );

                        DrawRectangle(
                            xTijolo,
                            y + 1,
                            larguraTijolo,
                            4,
                            (Color){220, 130, 70, 255}
                        );

                        x += 40;
                    }
                }
            }

            // INIMIGOS

            if (!modoBoss && !transicaoBoss) {
                DrawBruxa(&bruxa);
                DrawCogumelo(&cogumelo);
            } else if (bossCriado) {
                DrawBoss(&boss);
                DrawSpike(spike);
            }

            // JOGADOR

            Texture2D texAtual = texPrincesa;

            if (jogador.estado == JUMP) {
                if (jogador.velocidadeY < -50) {
                    texAtual = jump[0];
                } else if (jogador.velocidadeY > 50) {
                    texAtual = jump[2];
                } else {
                    texAtual = jump[1];
                }
            }

            DrawTexturePro(
                texAtual,
                (Rectangle){0, 0, texAtual.width, texAtual.height},
                (Rectangle){jogador.posicao.x, jogador.posicao.y, 100, 100},
                (Vector2){50, 50},
                0,
                WHITE
            );

            DesenharTiros(texEstrela);

            // HUD

            DrawText(TextFormat("Vidas: %d", jogador.vidas), 30, 30, 30, RED);
            DrawFPS(30, 70);

            const char *textoKills = TextFormat("%d/%d", kills, META_KILLS);
            int larguraTextoKills = MeasureText(textoKills, 30);

            DrawText(
                textoKills,
                LARGURA_TELA - larguraTextoKills - 30,
                30,
                30,
                GOLD
            );

            const char *textoScore = TextFormat("Score: %d", score);
            int larguraTextoScore = MeasureText(textoScore, 30);

            DrawText(
                textoScore,
                LARGURA_TELA - larguraTextoScore - 30,
                70,
                30,
                GOLD
            );

            if (modoBoss) {
                DrawText("BOSS FINAL!", LARGURA_TELA / 2 - 110, 30, 30, RED);
            }

            if (transicaoBoss) {
                float alpha = 1.0f;

                if (tempoTransicaoBoss < TEMPO_SPAWN_BOSS) {
                    alpha = tempoTransicaoBoss / TEMPO_SPAWN_BOSS;
                }

                if (alpha > 1.0f) {
                    alpha = 1.0f;
                }

                if (alpha < 0.0f) {
                    alpha = 0.0f;
                }

                DrawRectangle(
                    0,
                    0,
                    LARGURA_TELA,
                    ALTURA_TELA,
                    (Color){0, 0, 0, (unsigned char)(alpha * 160)}
                );

                DrawText(
                    "O BOSS ESTA CHEGANDO...",
                    LARGURA_TELA / 2 - 250,
                    ALTURA_TELA / 2 - 30,
                    36,
                    WHITE
                );
            }
        }

        else if (estado == GAMEOVER) {
            DrawTexturePro(
                texGameOver,
                (Rectangle){0, 0, texGameOver.width, texGameOver.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );

            const char *textoFinal = TextFormat("%d", score);
            int tamanhoFonte = 55;
            int larguraTexto = MeasureText(textoFinal, tamanhoFonte);
            Color rosaScore = (Color){255, 70, 170, 255};

            DrawText(
                textoFinal,
                (LARGURA_TELA * 0.575f) - (larguraTexto / 2),
                ALTURA_TELA * 0.525f,
                tamanhoFonte,
                rosaScore
            );
        }

        else if (estado == YOUWIN) {
            DrawTexturePro(
                texYouWin,
                (Rectangle){0, 0, texYouWin.width, texYouWin.height},
                (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
                (Vector2){0, 0},
                0,
                WHITE
            );

            const char *textoFinal = TextFormat("%d", score);
            int tamanhoFonte = 45;
            int larguraTexto = MeasureText(textoFinal, tamanhoFonte);
            Color rosaScore = (Color){255, 70, 170, 255};

            DrawText(
                textoFinal,
                (LARGURA_TELA * 0.575f) - (larguraTexto / 2),
                ALTURA_TELA * 0.525f,
                tamanhoFonte,
                rosaScore
            );
        }

        EndDrawing();
    }

    // LIBERAÇÃO DE RECURSOS
    if (bossCriado) {
        UnloadBoss(&boss);
    }

    UnloadBruxa(&bruxa);
    UnloadCogumelo(&cogumelo);
    UnloadSpike(&spike);

    UnloadTexture(texFundo);
    UnloadTexture(texFundoBoss);
    UnloadTexture(texPrincesa);
    UnloadTexture(texEstrela);
    UnloadTexture(texMenu);
    UnloadTexture(texObjetivo);
    UnloadTexture(texGameOver);
    UnloadTexture(texYouWin);

    for (int i = 0; i < 3; i++) {
        UnloadTexture(jump[i]);
    }

    UnloadMusicStream(musica);
    UnloadMusicStream(musicaBoss);
    CloseAudioDevice();

    LiberarRanking();

    CloseWindow();

    return 0;
}
