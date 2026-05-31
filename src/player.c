#include "player.h"
#include <stdbool.h>

void AtualizarJogador(Jogador *jogador, Plataforma *plataformas, int qtd, float dt) {

    //MOVIMENTO HORIZONTAL
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) jogador->posicao.x -= VELOCIDADE_HORIZONTAL * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) jogador->posicao.x += VELOCIDADE_HORIZONTAL * dt;

    //PULO 
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && jogador->podePular) {
        jogador->velocidadeY = -FORCA_PULO;
        jogador->podePular   = false;
    }

    //GRAVIDADE 
    jogador->velocidadeY   += GRAVIDADE * dt;
    jogador->posicao.y     += jogador->velocidadeY * dt;
    jogador->podePular      = false;

    //HITBOX DO JOGADOR 
    Rectangle playerRect = {
        jogador->posicao.x - 50,
        jogador->posicao.y - 50,
        100,
        100
    };
    

    //COLISÃO COM PLATAFORMAS
    for (int i = 0; i < qtd; i++) {

        Plataforma *p = &plataformas[i];

        if (!p->bloqueia) continue;
        //Só continua se houver colisão, ou seja, se o jogador não está colidindo com aquela plataforma, "pula pra próxima"
        if (!CheckCollisionRecs(playerRect, p->area)) continue;

        //Quanto o jogador está "dentro" de cada lado da plataforma

        //base do jogador - topo da plataforma
        float penetracaoCima     = (playerRect.y + playerRect.height) - p->area.y;

        //base da plataforma - topo do jogador
        float penetracaoBaixo    = (p->area.y + p->area.height) - playerRect.y;

        //lado direito do jogador - lado esquerdo plataforma
        float penetracaoEsquerda = (playerRect.x + playerRect.width) - p->area.x;

        //lado direito da plataforma - lado esquerdo do jogador
        float penetracaoDireita  = (p->area.x + p->area.width) - playerRect.x;

        //Resolve pelo lado com MENOR penetração 
        if (penetracaoCima <= penetracaoBaixo &&
            penetracaoCima <= penetracaoEsquerda &&
            penetracaoCima <= penetracaoDireita) {

            // Pousou em cima

            //Como p->area.y é o topo da plataforma, e o jogador tem altura 100, o centro dele precisa ficar 50 pixels acima do topo
            jogador->posicao.y   = p->area.y - 50;
            jogador->velocidadeY = 0;
            jogador->podePular   = true;
        }
        else if (penetracaoBaixo <= penetracaoCima &&
                penetracaoBaixo <= penetracaoEsquerda &&
                penetracaoBaixo <= penetracaoDireita) {

            // Bateu embaixo
            //Agora como ele bateu em baixo seu centro tem que ficar 50 pixels abaixo da base, assim deixa ele totalmente abaixo dela
            jogador->posicao.y   = p->area.y + p->area.height + 50;
            jogador->velocidadeY = 0;
        }
        else if (penetracaoEsquerda <= penetracaoDireita) {

            // Bateu no lado esquerdo da plataforma
            //Como o jogador tem largura 100 e a posição dele é o centro, ele precisa ficar 50 pixels antes da plataforma
            jogador->posicao.x = p->area.x - 50;
        }
        else {

            // Bateu no lado direito da plataforma
            //Como o jogador tem largura 100 e a posição dele é o centro, ele precisa ficar 50 pixels depois da plataforma
            jogador->posicao.x = p->area.x + p->area.width + 50;
        }

        // Atualiza hitbox após mover
        playerRect.x = jogador->posicao.x - 50;
        playerRect.y = jogador->posicao.y - 50;
    }
    //ESTADOS DE ANIMAÇÃO
    if (!jogador->podePular) {
        jogador->estado = JUMP;
    }
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) ||
             IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
        jogador->estado = RUN;
    }
    else {
        jogador->estado = IDLE;
    }
}