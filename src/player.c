#include "player.h"
#include <stdbool.h>

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