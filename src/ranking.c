#include "ranking.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static NoRanking *inicio = NULL;
static int rankingCarregado = 0;

static void InserirRankingNaLista(const char *nome, int score) {
    NoRanking *novo = malloc(sizeof(NoRanking));

    if (novo == NULL) {
        printf("Erro ao alocar memoria!\n");
        return;
    }

    strncpy(novo->nome, nome, 29);
    novo->nome[29] = '\0';
    novo->score = score;
    novo->prox = NULL;

    if (inicio == NULL || score > inicio->score) {
        novo->prox = inicio;
        inicio = novo;
        return;
    }

    NoRanking *atual = inicio;

    while (atual->prox != NULL && atual->prox->score >= score) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
}

static void CarregarRankingDoArquivo(void) {
    if (rankingCarregado) return;

    FILE *arquivo = fopen("ranking.txt", "r");

    if (arquivo == NULL) {
        rankingCarregado = 1;
        return;
    }

    char linha[100];
    char nome[30];
    int score;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%29[^-] - Score: %d", nome, &score) == 2) {
            int tam = strlen(nome);

            while (tam > 0 && nome[tam - 1] == ' ') {
                nome[tam - 1] = '\0';
                tam--;
            }

            InserirRankingNaLista(nome, score);
        }
    }

    fclose(arquivo);
    rankingCarregado = 1;
}

void SalvarRanking(const char *nome, int score) {
    CarregarRankingDoArquivo();

    InserirRankingNaLista(nome, score);

    FILE *arquivo = fopen("ranking.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir ranking.txt\n");
        return;
    }

    NoRanking *atual = inicio;

    while (atual != NULL) {
        fprintf(arquivo, "%s - Score: %d\n", atual->nome, atual->score);
        atual = atual->prox;
    }

    fclose(arquivo);
}

void LiberarRanking(void) {
    NoRanking *atual = inicio;

    while (atual != NULL) {
        NoRanking *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    inicio = NULL;
    rankingCarregado = 0;
}

