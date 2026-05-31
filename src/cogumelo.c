#include "cogumelo.h"

#define INTERVALO_APARICAO_COGUMELO 2.0f 
#define LARGURA_COGUMELO 95
#define ALTURA_COGUMELO  95

/* INICIALIZAÇÃO*/

/*InitCogumelo - Prepara o cogumelo para ser usado no jogo pela primeira vez
 Assim como a bruxa, essa função é chamada uma única vez no início do jogo.
 Ela define o estado inicial de todos os campos da struct CogumeloRei.
 Pontos importantes:
   - ativo = false: o cogumelo começa invisível, esperando o cooldown.
   - tempoCooldownAparicao: quanto tempo (em segundos) até a primeira aparição.
   - velocidade = 160.0f: pixels por segundo que ele anda para a esquerda.
   - posicao: começa fora da tela pelo lado direito (largura + 80), já
     alinhado com o chão. O "chaoY - ALTURA/2" centraliza verticalmente o
     sprite na linha do chão, já que o pivô de desenho fica no centro.
   - vidas = 2: o cogumelo aguenta 2 acertos antes de ser derrotado.

 LoadTexture("caminho") — função do Raylib
   Carrega a imagem, devolvendo uma struct Texture2D. 
   Só é chamada uma vez que é na inicialização, e não dentro do loop de jogo a cada frame */
void InitCogumelo(CogumeloRei *c, int largura, float chaoY) {
    c->ativo                 = false;
    c->tempoCooldownAparicao = INTERVALO_APARICAO_COGUMELO;
    c->velocidade            = 160.0f;
    c->posicao               = (Vector2){(float)largura + 80, chaoY - ALTURA_COGUMELO / 2.0f};
    c->textura = LoadTexture("imagens/cogumelo_rei.png");
    c->vidas = 2;
}

/*UPDATE*/

/* UpdateCogumelo - Atualiza toda a lógica do cogumelo a cada frame

 Essa função é chamada uma vez por frame dentro do loop principal do jogo.
 Ela tem duas responsabilidades principais:

   1. COOLDOWN / APARIÇÃO: Se o cogumelo estiver inativo, decrementa o
      tempoCooldownAparicao a cada frame. Quando chega a zero, reposiciona
      o cogumelo fora da tela pela direita e o ativa. O "return" no final
      do bloco inativo é importante: evita que o resto da função (movimento)
      rode enquanto ele ainda está esperando para aparecer.

   2. MOVIMENTO: Enquanto ativo, subtrai a velocidade * dt da posição X,
      fazendo ele andar para a esquerda. Quando sai completamente pela
      borda esquerda, volta a ser inativo e reinicia o cooldown.

 GetFrameTime() — função do Raylib
   Retorna o tempo em segundos que o último frame demorou para ser
   processado (delta time / dt). Multiplicar a velocidade por dt garante
   que o cogumelo ande na mesma velocidade em qualquer máquina*/
void UpdateCogumelo(CogumeloRei *c, int largura, float chaoY) {
    float dt = GetFrameTime();

    if (!c->ativo) {
        c->tempoCooldownAparicao -= dt;
        if (c->tempoCooldownAparicao <= 0) {
            /* Spawna pela direita, posicionado no chão */
            c->posicao = (Vector2){(float)largura + 80, chaoY - ALTURA_COGUMELO / 2.0f};
            c->ativo   = true;
            c->vidas   = 2;
        }
        return; /* Não executa o movimento enquanto estiver inativo */
    }

    /* Move para a esquerda: subtrai porque X cresce para a direita */
    c->posicao.x -= c->velocidade * dt;

    /* Se saiu completamente pela borda esquerda, desativa e reinicia cooldown */
    if (c->posicao.x + LARGURA_COGUMELO < 0) {
        c->ativo                 = false;
        c->tempoCooldownAparicao = INTERVALO_APARICAO_COGUMELO;
    }
}

/*DRAW*/

/*DrawCogumelo - Desenha o cogumelo e sua barra de vida na tela

 Essa função só cuida da renderização, sem nenhuma lógica de jogo.
 É chamada uma vez por frame.

 Se o cogumelo não estiver ativo, sai imediatamente com "return" —
 não há nada para desenhar.

 DrawTexturePro() — função do Raylib
   É a versão mais completa de desenho de textura. Permite controlar:
     - source (Rectangle): qual parte da textura usar. Aqui usamos a
       textura inteira: (0, 0, largura, altura).
     - dest (Rectangle): onde e com qual tamanho desenhar na tela.
       Usamos LARGURA_COGUMELO e ALTURA_COGUMELO (95x95 pixels).
     - origin (Vector2): ponto de pivô para posicionamento e rotação.
       Usar (largura/2, altura/2) faz com que c->posicao.x e c->posicao.y
       representem o CENTRO do sprite, não o canto superior esquerdo.
       Isso facilita alinhar o cogumelo com o chão e centralizar a barra
       de vida em cima dele.
     - rotation: ângulo em graus. Zero = sem rotação.
     - tint: cor multiplicada na textura. WHITE = sem alteração de cor.

 DrawRectangle() — função do Raylib
   Desenha um retângulo sólido na posição e tamanho informados.
   Aqui é usada duas vezes para montar a barra de vida:
     1. Um retângulo cinza escuro de 60px (largura total = vida máxima).
     2. Um retângulo vermelho em cima, com largura = 30 * vidas restantes.
        Com 2 vidas = 60px, com 1 vida = 30px. Vai encolhendo conforme
        o cogumelo leva dano */
void DrawCogumelo(CogumeloRei *c) {
    if (!c->ativo) return;

    DrawTexturePro(
        c->textura,
        (Rectangle){0, 0, c->textura.width, c->textura.height}, /* Textura inteira */
        (Rectangle){c->posicao.x, c->posicao.y, LARGURA_COGUMELO, ALTURA_COGUMELO},
        (Vector2){LARGURA_COGUMELO / 2.0f, ALTURA_COGUMELO / 2.0f}, /* Pivô no centro */
        0.0f,  /* Sem rotação */
        WHITE  /* Sem alteração de cor */
    );

    /* Fundo cinza: representa a barra de vida máxima (60px = 2 vidas × 30) */
    DrawRectangle(
        c->posicao.x - 30,  /* Centralizado horizontalmente em relação ao cogumelo */
        c->posicao.y - 85,  /* 85px acima do centro do sprite */
        60,
        8,
        DARKGRAY
    );

    /* Parte vermelha: encolhe conforme perde vida (30px por vida restante) */
    DrawRectangle(
        c->posicao.x - 30,
        c->posicao.y - 85,
        30 * c->vidas, /* 2 vidas = 60px | 1 vida = 30px | 0 vidas = sumiu */
        8,
        RED
    );
}

/*COLISÃO*/

/*CheckCogumeloCollision - Verifica se o jogador colidiu com o cogumelo

 Recebe um Rectangle do jogador e checa se ele se sobrepõe ao Rectangle
 do cogumelo. Retorna true se houve colisão, false caso contrário.

 Se o cogumelo estiver inativo, retorna false imediatamente — não faz
 sentido checar colisão com algo que não está na tela.

 O Rectangle do cogumelo é montado manualmente aqui porque c->posicao
 representa o CENTRO do sprite (por causa do pivô no DrawTexturePro).
 Para o Rectangle de colisão, o Raylib espera o CANTO SUPERIOR ESQUERDO,
 então subtraímos metade da largura e altura para converter:

   canto superior esquerdo X = posicao.x - LARGURA / 2
   canto superior esquerdo Y = posicao.y - ALTURA  / 2

 CheckCollisionRecs(rec1, rec2) — função do Raylib
   Recebe dois Rectangles e retorna true se eles se sobrepõem (colidem).
   Internamente ela checa se os retângulos se intersectam nos dois eixos
   (X e Y) */
bool CheckCogumeloCollision(CogumeloRei *c, Rectangle playerRect) {
    if (!c->ativo) return false;

    /* Converte posição central para canto superior esquerdo, que o Raylib espera */
    Rectangle cogumeloRect = {
        c->posicao.x - LARGURA_COGUMELO / 2.0f,
        c->posicao.y - ALTURA_COGUMELO  / 2.0f,
        LARGURA_COGUMELO,
        ALTURA_COGUMELO
    };

    return CheckCollisionRecs(cogumeloRect, playerRect);
}

/*LIBERAÇÃO DE RECURSOS*/

/*UnloadCogumelo - Libera da memória a textura carregada pelo cogumelo

 Toda textura carregada com LoadTexture() ocupa espaço na memória.
 Quando o jogo fechar, é necessário liberar esse espaço manualmente.

 UnloadTexture(texture) — função do Raylib
   Descarrega a textura da memória. Após essa chamada,
   o handle Texture2D não deve mais ser usado para desenhar nada*/
void UnloadCogumelo(CogumeloRei *c) {
    UnloadTexture(c->textura);
}