#include "tiro.h"
#include <math.h>

#define MAX_TIROS 50

typedef struct {
    //Onde o tiro está na tela
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
    float rotacao;
} Tiro;

static Tiro tiros[MAX_TIROS];

//Todos os tiros iniciam desativados
void InitTiros(void) {
    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }
}


void DispararTiro(Vector2 origem, Vector2 destino) {

    Vector2 direcao = {
        destino.x - origem.x,
        destino.y - origem.y
    };


    float tamanho = sqrt(direcao.x * direcao.x + direcao.y * direcao.y);

    if (tamanho == 0) return;

    direcao.x /= tamanho;
    direcao.y /= tamanho;

    for (int i = 0; i < MAX_TIROS; i++) {

        if (!tiros[i].ativo) {
            tiros[i].ativo       = true;
            tiros[i].posicao     = origem;
            tiros[i].rotacao     = 0;
            tiros[i].velocidade  = (Vector2){direcao.x * 300, direcao.y * 300}; //No final velocidade = direção normalizada x 300
            break;
        }
    }
}

int ContarTirosAtivos() {
    int ativos = 0;

    for (int i = 0; i < MAX_TIROS; i++) {
        if (tiros[i].ativo) ativos++;
    }

    return ativos;
}

void AtualizarTiros(float dt, int largura, int altura) {

    for (int i = 0; i < MAX_TIROS; i++) {

        if (!tiros[i].ativo) continue;

        //nova posição = posição atual + velocidade x tempo
        tiros[i].posicao.x += tiros[i].velocidade.x * dt;
        tiros[i].posicao.y += tiros[i].velocidade.y * dt;
        tiros[i].rotacao   += 200 * dt;

        //Saiu da tela
        if (tiros[i].posicao.x < 0 || tiros[i].posicao.x > largura ||
            tiros[i].posicao.y < 0 || tiros[i].posicao.y > altura) {
            tiros[i].ativo = false;
        }
    }
}

void DesenharTiros(Texture2D textura) {

    for (int i = 0; i < MAX_TIROS; i++) {

        if (!tiros[i].ativo) continue;

        DrawTexturePro(
            textura,
            (Rectangle){0, 0, textura.width, textura.height},
            (Rectangle){tiros[i].posicao.x, tiros[i].posicao.y, 30, 30},
            (Vector2){15, 15},
            tiros[i].rotacao,
            WHITE
        );
    }
}


//Permite a main saber quantos tiros existem no vetor
int GetMaxTiros(void) {
    return MAX_TIROS;
}

//Entrega pra nossa main a hitbox do tiro, e se ele está ativo
void GetTiroRect(int indiceTiro, Rectangle *hitboxTiro, bool *tiroEstaAtivo) 
{

    if (indiceTiro < 0 || indiceTiro >= MAX_TIROS) {
        *tiroEstaAtivo = false;
        return;
    }

    *tiroEstaAtivo = tiros[indiceTiro].ativo;

    hitboxTiro->x = tiros[indiceTiro].posicao.x - 15;
    hitboxTiro->y = tiros[indiceTiro].posicao.y - 15;
    hitboxTiro->width = 30;
    hitboxTiro->height = 30;
}

void DesativarTiro(int indiceTiro) {

    if (indiceTiro >= 0 && indiceTiro < MAX_TIROS) {
        tiros[indiceTiro].ativo = false;
    }

}
