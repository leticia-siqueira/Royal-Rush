#include "raylib.h"
#include "objetos.h"
#include <stdio.h>

#define G 300
#define speed_jump 350.0f
#define speed_hor 200.f

typedef struct{
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct{
    Rectangle rect;
    bool blocking;
    Color color;
} EnvItem;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta){

    if(IsKeyDown(KEY_LEFT)) player->position.x -= speed_hor * delta;
    if(IsKeyDown(KEY_RIGHT)) player->position.x += speed_hor * delta;

    if((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP)) && player->canJump){
        player->speed = -speed_jump;
        player->canJump = false;
    }

    player->speed += G * delta;
    player->position.y += player->speed * delta;

    player->canJump = false;

    for(int i = 0; i < envItemsLength; i++){
        EnvItem *ei = &envItems[i];

        if(ei->blocking){
            if(player->position.x > ei->rect.x &&
               player->position.x < ei->rect.x + ei->rect.width &&
               player->position.y >= ei->rect.y &&
               player->position.y <= ei->rect.y + 10){

                player->position.y = ei->rect.y;
                player->speed = 0;
                player->canJump = true;
            }
        }
    }
}

int main(void){

    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Bubble Rush");

    Texture2D princesa = LoadTexture("imagens/princesa.png");
    Texture2D fundo = LoadTexture("imagens/fundo.png");

    Player player = {0};
    player.position = (Vector2){400, 200};
    player.speed = 0;
    player.canJump = false;

    EnvItem envItems[] = {
        {{0, 577, screenWidth, 300}, true, GRAY},
        {{929, 412, 144, 38}, true, RED}
    };

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    SpikeBall spike;
    InitSpike(&spike);

    int vidas = 3;
    float tempoDano = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()){

        float delta = GetFrameTime();

        UpdatePlayer(&player, envItems, envItemsLength, delta);
        UpdateSpike(&spike);

        if (tempoDano > 0) tempoDano -= delta;

        Rectangle playerRect = {
            player.position.x - 50,
            player.position.y - 50,
            100,
            100
        };

        if (CheckSpikeCollision(spike, playerRect) && tempoDano <= 0) {
            vidas--;
            tempoDano = 1.0f;
            spike.position.x = screenWidth;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        for (int i = 0; i < envItemsLength; i++){
            DrawRectangleRec(envItems[i].rect, envItems[i].color);
        }
        DrawTexturePro(
            fundo,
            (Rectangle){0, 0, fundo.width, fundo.height},
            (Rectangle){0, 0, screenWidth, screenHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );


        DrawSpike(spike);

        DrawTexturePro(
            princesa,
            (Rectangle){0, 0, princesa.width, princesa.height},
            (Rectangle){player.position.x, player.position.y, 100, 100},
            (Vector2){50, 50},
            0.0f,
            WHITE
        );

        DrawText(TextFormat("Vidas: %d", vidas), 30, 30, 30, RED);

        EndDrawing();
    }

    UnloadSpike(&spike);
    UnloadTexture(fundo);
    UnloadTexture(princesa);

    CloseWindow();

    return 0;
}

