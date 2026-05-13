#include "ranking.h"
#include <stdio.h>

void SalvarRanking(const char *nome, int score) {
    FILE *arquivo = fopen("ranking.txt", "a");

    if (arquivo == NULL) {
        printf("Erro ao abrir ranking.txt\n");
        return;
    }

    fprintf(arquivo, "%s - Score: %d\n", nome, score);

    fclose(arquivo);
}

