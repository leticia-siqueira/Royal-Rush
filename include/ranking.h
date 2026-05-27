#ifndef RANKING_H
#define RANKING_H

typedef struct NoRanking {
    char nome[30];
    int score;
    struct NoRanking *prox;
} NoRanking;

void SalvarRanking(const char *nome, int score);
void LiberarRanking(void);

#endif

