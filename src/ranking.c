#include "ranking.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ponteiro para o início da lista
static NoRanking *inicio = NULL;

void SalvarRanking(const char *nome, int score) {


//Cria um novo nó

    NoRanking *novo = malloc(sizeof(NoRanking));

    // Verifica se a memória foi alocada
    if (novo == NULL) {
        printf("Erro ao alocar memória!\n");
        return;
    }


//Preenche os dados

    strcpy(novo->nome, nome);

    novo->score = score;

    novo->prox = NULL;

    if (inicio == NULL) {
        inicio = novo;
    }

    else {
        NoRanking *atual = inicio;
        NoRanking *anterior = NULL;

        while (atual!= NULL && atual->score > score) {
            anterior = atual;
            atual = atual->prox;
        }

        if(anterior == NULL){
            novo->prox = inicio;
            inicio = novo;
        }
        else{
            anterior->prox = novo;
            novo->prox = atual;
        }

    }


// Salva no arquivo

    FILE *arquivo = fopen("ranking.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir ranking.txt\n");
        return;
    }


    NoRanking *atual = inicio;

    while (atual != NULL) {

        fprintf(
            arquivo,
            "%s - Score: %d\n",
            atual->nome,
            atual->score
        );

        atual = atual->prox;
    }

    fclose(arquivo);
}

void LiberarRanking() {

    NoRanking *atual = inicio;

    while (atual != NULL) {

        NoRanking *temp = atual;

        atual = atual->prox;

        free(temp);
    }

    inicio = NULL;
}