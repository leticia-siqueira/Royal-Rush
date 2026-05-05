#include "bruxa.h"

void InitBruxa(BruxaVoando *bruxa) {
    bruxa->position     = (Vector2){1280, 150};  // começa pela direita, altura aérea
    bruxa->speed        = 180;
    bruxa->active       = true;
    bruxa->tempoCooldown = 0;
    bruxa->esperando    = false;

    bruxa->texture = LoadTexture("imagens/BruxaVoando.png");
}

void UpdateBruxa(BruxaVoando *bruxa) {

    // Se está esperando, conta o cooldown de 3 segundos
    if (bruxa->esperando) {
        bruxa->tempoCooldown -= GetFrameTime();

        if (bruxa->tempoCooldown <= 0) {
            // Voltou! Reposiciona no lado direito
            bruxa->position.x = 1280;
            bruxa->esperando  = false;
        }
        return; // não move enquanto espera
    }

    // Move da direita para a esquerda
    bruxa->position.x -= bruxa->speed * GetFrameTime();

    // Chegou ao fim da tela — inicia cooldown de 3 segundos
    if (bruxa->position.x < -80) {
        bruxa->esperando     = true;
        bruxa->tempoCooldown = 3.0f;
    }
}

void DrawBruxa(BruxaVoando bruxa) {

    // Não desenha enquanto está esperando fora da tela
    if (bruxa.esperando) return;

    DrawTexturePro(
        bruxa.texture,
        (Rectangle){0, 0, bruxa.texture.width, bruxa.texture.height},
        (Rectangle){bruxa.position.x, bruxa.position.y, 110, 110},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

bool CheckBruxaCollision(BruxaVoando bruxa, Rectangle playerRect) {

    if (bruxa.esperando) return false; // sem colisão enquanto espera

    Rectangle bruxaRect = {
        bruxa.position.x,
        bruxa.position.y,
        110,
        110
    };

    return CheckCollisionRecs(bruxaRect, playerRect);
}

void UnloadBruxa(BruxaVoando *bruxa) {
    UnloadTexture(bruxa->texture);
}