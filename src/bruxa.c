#include "bruxa.h"

#include <math.h>

#define INTERVALO_APARICAO_BRUXA 2.7f
#define INTERVALO_TIRO_BRUXA 1.8f
#define VELOCIDADE_TIRO_BRUXA 280.0f
#define VELOCIDADE_TIRO_ROTACAO 250.0f

/*FUNÇÕES AUXILIARES*/

/* NormalizarVetor - Transforma um vetor qualquer em um vetor de comprimento 1
Como a bruxa atira na direção do jogador. Você calcula a diferença de posição (dx, dy), 
mas esse vetor pode ter qualquer tamanho. Com essa função se mantém apenas a direção, ignorando a distância. 
Porque quando se multiplica pela velocidade do tiro, o tiro anda sempre na mesma velocidade 
independente de onde o jogador estiver. Porque quando fizesse o calculo da distancia entre abruxa e o jogador e multiplicasse 
pela velocidade do tiro, quanto mais longe mais rápido ficaria. 
Então, a solução é dividir o vetor pelo próprio comprimento, fazendo ele ter comprimento 1 para ter a mesma velocidade sempre 
formula usada: comprimento = sqrt(x² + y²) Pitágoras
Se o comprimento for zero (vetor nulo), devolvemos (1, 0) por segurança para não dividir por zero e causar um crash.
*/
static Vector2 NormalizarVetor(Vector2 vetor) {
    float tamanho = sqrtf(vetor.x * vetor.x + vetor.y * vetor.y);

    if (tamanho == 0) {
        return (Vector2){1, 0};
    }

    return (Vector2){
        vetor.x / tamanho,
        vetor.y / tamanho
    };
}

/*INICIALIZAÇÃO */

/*InitBruxa - Prepara a bruxa para ser usada no jogo pela primeira vez
Essa função é chamada uma uma vez no início do jogo. Ela coloca todos
os campos da struct Bruxa em um estado inicial seguro, para que nenhum
valor de lixo da memória cause comportamento estranho.
Pontos importantes:
- bruxa->ativa = false: a bruxa começa escondida, fora da tela.
- tempoCooldownAparicao: quanto tempo até ela aparecer pela primeira vez.
- vidas = 3: ela aguenta 3 acertos antes de ser derrotada.
- posicao = (-200, 0): coordenada fora da tela, invisível ao jogador.
- O loop zera todos os tiros (ativo = false), garantindo que nenhum tiro
  apareça do nada na tela.

LoadTexture("caminho") — função do Raylib
   Carrega uma imagem e envia para a GPU, devolvendo uma struct
   Texture2D. A partir daí, você usa esse handle para desenhar a imagem
   na tela. */
void InitBruxa(Bruxa *bruxa) {
    bruxa->ativa = false;
    bruxa->atirando = false;

    bruxa->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
    bruxa->tempoAtirando = 0.0f;
    bruxa->tempoAnimAtaque = 0.0f;

    bruxa->tirosDados = 0;
    bruxa->vidas = 3;

    bruxa->posicao = (Vector2){-200, 0};
    bruxa->velocidade = (Vector2){-120, 0};

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        bruxa->tiros[i].ativo = false;
    }

    bruxa->texVoando  = LoadTexture("imagens/bruxa_voando.png");
    bruxa->texAtirando = LoadTexture("imagens/bruxa_atirando.png");
    bruxa->texTiro    = LoadTexture("imagens/tiro_bruxa.png");
}

/* EscolherAlturaSeguraBruxa - Define em qual altura vertical a bruxa vai aparecer
Como queira variar o jogo, a bruxa não aparece sempre na mesma altura. Temos 4
alturas pré-definidas como porcentagens da tela (ex: 18% do topo, 53% e etc...)
A cada chamada, a função escolhe a próxima da lista usando um índice fixo.
- static int indiceAltura significa que essa variável persiste entre as
chamadas da função (diferente de uma variável local normal que é recriada
do zero a cada chamada). Assim, a cada vez que a bruxa reaparece ela sobe
um degrau na lista, e ao chegar no fim volta ao começo.

Usar porcentagens da tela (alturaTela * 0.18f) em vez de pixels fixos
para o jogo funcionar corretamente em resoluções diferentes.*/
static float EscolherAlturaSeguraBruxa(int alturaTela) {
    static int indiceAltura = 0;

    float alturasSeguras[4] = {
        alturaTela * 0.18f,
        alturaTela * 0.53f,
        alturaTela * 0.23f,
        alturaTela * 0.56f
    };

    float alturaEscolhida = alturasSeguras[indiceAltura];

    indiceAltura++;

    if (indiceAltura >= 4) {
        indiceAltura = 0;
    }

    return alturaEscolhida;
}

/*UPDATE*/

/* UpdateBruxa - Atualiza toda a lógica da bruxa a cada frame

 Essa é a função principal da bruxa. Ela é chamada uma vez por frame dentro
 do loop principal do jogo e ela faz 3 coisas:

   1. APARIÇÃO: Se a bruxa estiver inativa, decrementa o cooldown. Quando
      chega a zero, reposiciona a bruxa para dentro da tela do jogador e a ativa.

   2. MOVIMENTO E DISPARO: Enquanto ativa, a bruxa se move da direita para esquerda. 
      A cada INTERVALO_TIRO_BRUXA segundos, ela dispara um
      tiro em direção ao jogador. Quando sai completamente pela esquerda
      da tela, volta a ficar inativa.

   3. TIROS: Cada tiro ativo tem sua posição atualizada com base na velocidade
      e também gira visualmente. Tiros que saem da tela são desativados.
 
GetFrameTime() — função do Raylib
Retorna o tempo em segundos que o último frame demorou para ser processado
(o delta T da fisica - dt). Multiplicar velocidades por dt é essencial
para que o jogo rode na mesma velocidade em qualquer máquina */
void UpdateBruxa(Bruxa *bruxa, Vector2 posJogador, int larguraTela, int alturaTela) {
    float dt = GetFrameTime();

    /* APARIÇÃO DA BRUXA */

    if (!bruxa->ativa) {
        
        bruxa->tempoCooldownAparicao -= dt;

        if (bruxa->tempoCooldownAparicao <= 0) {
            float yAereo = EscolherAlturaSeguraBruxa(alturaTela);

            /* Reposiciona além da borda direita, fora do campo de visão */
            bruxa->posicao = (Vector2){(float)larguraTela + 60, yAereo};
            bruxa->velocidade = (Vector2){-170, 0};

            bruxa->ativa = true;
            bruxa->atirando = false;

            bruxa->tirosDados = 0;
            bruxa->tempoAtirando = 0.0f;
            bruxa->tempoAnimAtaque = 0.0f;

            bruxa->vidas = 3;
        }
    }

    /* --- MOVIMENTO E DISPARO --- */

    if (bruxa->ativa) {

        /*Controla a animação de ataque: quando bruxa->atirando é true,
        acumula o tempo. Após 0.4 segundos, volta ao sprite de voo normal.
        Isso é basicamente um timer de animação*/
        if (bruxa->atirando) {
            bruxa->tempoAnimAtaque += dt;

            if (bruxa->tempoAnimAtaque >= 0.4f) {
                bruxa->atirando = false;
                bruxa->tempoAnimAtaque = 0.0f;
            }
        }

        /* Move a bruxa para a esquerda: posição += velocidade * tempo */
        bruxa->posicao.x += bruxa->velocidade.x * dt;

        /* Se saiu completamente pela esquerda, desativa e reinicia o cooldown */
        if (bruxa->posicao.x + 120 < 0) {
            bruxa->ativa = false;
            bruxa->tempoCooldownAparicao = INTERVALO_APARICAO_BRUXA;
        }

        /*Lógica de disparo: só tira se ainda não atingiu o limite de tiros
        (MAX_TIROS_BRUXA). Acumula tempo e dispara quando o intervalo
        for atingido. Procura o primeiro slot livre no array de tiros */
        if (bruxa->tirosDados < MAX_TIROS_BRUXA) {
            bruxa->tempoAtirando += dt;

            if (bruxa->tempoAtirando >= INTERVALO_TIRO_BRUXA) {
                bruxa->tempoAtirando = 0.0f;

                for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
                    if (!bruxa->tiros[i].ativo) {

                        /*Calcula a direção do tiro subtraindo as posições.
                        O resultado é um vetor apontando da bruxa para o
                        jogador. E ele normaliza pelo vetor garantindo que a velocidade seja
                        sempre VELOCIDADE_TIRO_BRUXA, independente da
                        distância */
                        Vector2 direcao = {
                            posJogador.x - bruxa->posicao.x,
                            posJogador.y - bruxa->posicao.y
                        };

                        direcao = NormalizarVetor(direcao);

                        bruxa->tiros[i].ativo = true;
                        bruxa->tiros[i].posicao = bruxa->posicao;

                        bruxa->tiros[i].velocidade = (Vector2){
                            direcao.x * VELOCIDADE_TIRO_BRUXA,
                            direcao.y * VELOCIDADE_TIRO_BRUXA
                        };

                        bruxa->tiros[i].rotacao = 0.0f;

                        bruxa->tirosDados++;
                        bruxa->atirando = true;

                        break; /* Só dispara um tiro por vez, para no primeiro slot livre */
                    }
                }
            }
        }
    }

    /* --- ATUALIZAÇÃO DOS TIROS DA BRUXA --- */

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!bruxa->tiros[i].ativo) {
            continue; /* Pula tiros inativos para não processar à toa */
        }

        /* Move o tiro com base na sua velocidade */
        bruxa->tiros[i].posicao.x += bruxa->tiros[i].velocidade.x * dt;
        bruxa->tiros[i].posicao.y += bruxa->tiros[i].velocidade.y * dt;

        /* Gira o sprite do tiro para dar um efeito visual giratório */
        bruxa->tiros[i].rotacao += VELOCIDADE_TIRO_ROTACAO * dt;

        /* Desativa o tiro se ele sair dos limites da tela (com folga de 40px) */
        if (bruxa->tiros[i].posicao.x < -40 ||
            bruxa->tiros[i].posicao.x > larguraTela + 40 ||
            bruxa->tiros[i].posicao.y < -40 ||
            bruxa->tiros[i].posicao.y > alturaTela + 40) {

            bruxa->tiros[i].ativo = false;
        }
    }
}

/*DRAW*/

/*DrawBruxa - Desenha a bruxa e todos os seus tiros na tela

Essa função só cuida da renderização. Não tem nenhuma lógica de jogo propriamente.
Ela é chamada uma vez por frame, após o UpdateBruxa.

Ela ajeita a  ordem do desenho: o que é desenhado primeiro fica "atrás".
Por isso os tiros são desenhados antes da bruxa, para que ela apareça
por cima dos projéteis visualmente.

DrawTexturePro() — função do Raylib
É a versão mais completa de desenho de textura do Raylib. Permite:
- source (Rectangle): qual parte da textura usar (recorte/spritesheet)
- dest   (Rectangle): onde e com qual tamanho desenhar na tela
- origin (Vector2):   ponto de pivô para rotação e posicionamento
- rotation (float):   ângulo de rotação em graus
- tint   (Color):     cor multiplicada na textura (WHITE = sem alteração)

Usar (width/2, height/2) como origin faz a rotação e o posicionamento
acontecerem a partir do centro da imagem.

DrawRectangle() — função do Raylib
Desenha um retângulo sólido na tela. Usada aqui para a barra de vida:
primeiro um retângulo cinza escuro (fundo total), depois um vermelho 
em cima com largura proporcional às vidas restantes (20 * vidas).*/
void DrawBruxa(Bruxa *bruxa) {

    /* --- DESENHA OS TIROS --- */

    for (int i = 0; i < MAX_TIROS_BRUXA; i++) {
        if (!bruxa->tiros[i].ativo) {
            continue;
        }

        Texture2D texturaTiro = bruxa->texTiro;

        /*DrawTexturePro com origin no centro (15, 15) para que a rotação
        aconteça ao redor do meio do sprite de 30x30 pixels.*/
        DrawTexturePro(
            texturaTiro,
            (Rectangle){0, 0, texturaTiro.width, texturaTiro.height}, /* Usa a textura inteira */
            (Rectangle){bruxa->tiros[i].posicao.x, bruxa->tiros[i].posicao.y, 30, 30}, /* Tamanho na tela */
            (Vector2){15, 15},               /* Pivô no centro do sprite 30x30 */
            bruxa->tiros[i].rotacao,         /* Rotação atual do tiro */
            WHITE                            /* Sem tint, cor original */
        );
    }

    /* Se a bruxa não está ativa, não há nada mais para desenhar */
    if (!bruxa->ativa) {
        return;
    }

    /* DESENHA A BRUXA */

    /* Troca de sprite dependendo do estado: se está atirando, usa a textura
    de ataque; 
    caso contrário, usa a textura de voo normal */
    Texture2D texturaAtual;

    if (bruxa->atirando) {
        texturaAtual = bruxa->texAtirando;
    } else {
        texturaAtual = bruxa->texVoando;
    }

    DrawTexturePro(
        texturaAtual,
        (Rectangle){0, 0, texturaAtual.width, texturaAtual.height}, /* Textura inteira */
        (Rectangle){bruxa->posicao.x, bruxa->posicao.y, 120, 120}, /* 120x120 na tela */
        (Vector2){60, 60},  /* Pivô no centro: posicao.x/y é o centro da bruxa */
        0.0f,               /* Sem rotação */
        WHITE
    );

    /* BARRA DE VIDA */

    /* Fundo cinza: 60px de largura total, representando a vida máxima */
    DrawRectangle(
        bruxa->posicao.x - 30, /* Centralizado em relação à bruxa */
        bruxa->posicao.y - 85, /* 85px acima do centro da bruxa */
        60,
        8,
        DARKGRAY
    );

    /* Parte vermelha: 20px por vida restante (3 vidas = 60px, 1 vida = 20px) */
    DrawRectangle(
        bruxa->posicao.x - 30,
        bruxa->posicao.y - 85,
        20 * bruxa->vidas, /* Encolhe conforme perde vidas */
        8,
        RED
    );
}

/*LIBERAÇÃO DE RECURSOS*/

/* UnloadTexture(texture) — função do Raylib
Libera a meamória das texturas carregadas pela bruxa
Como toda textura carregada com LoadTexture() ocupa memória. 
Quando o jogo fechar (ou quando a bruxa não for mais necessária), é necessário
liberar essa memória com a função UnloadTexture().

Não fazer isso causa memory leak: a memória fica ocupada mesmo
depois do programa terminar. Se esquecer de liberar recursos
pode causar travamentos e crash */
void UnloadBruxa(Bruxa *bruxa) {
    UnloadTexture(bruxa->texVoando);
    UnloadTexture(bruxa->texAtirando);
    UnloadTexture(bruxa->texTiro);
}