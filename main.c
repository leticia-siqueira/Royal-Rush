#include "raylib.h"
#include <stdio.h>

#define G 300
#define speed_jump 350.0f
#define speed_hor 200.f

typedef struct{
    Vector2 position;
    float speed;
    bool canJump;
}Player;

typedef struct{
    Rectangle rect;
    bool blocking;
    Color color;
} EnvItem;

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta){

    // movimento horizontal:
    if(IsKeyDown(KEY_LEFT)) player->position.x -= speed_hor * delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += speed_hor * delta;

    //pulo:
    if(IsKeyDown(KEY_SPACE) && player->canJump){
        player->speed = -speed_jump; //começa subindo
        player->canJump = false;//garante que só pode pular de novo se colidir com o chão
    }

    //gravidade:
    player->speed += G * delta; //a velocidade vai aumentando com o tempo

    //movimento vertical:
    player->position.y += player->speed * delta; //move o persongem com base na velocidade

    //colisão com plataformas:
    player->canJump = false;

    for(int i = 0; i < envItemsLength; i++){
        EnvItem *ei = &envItems[i];

        if(ei->blocking){
            //verifica se o player tá em cima da plataforma
            //obs: no retângulo precisamos da largura, pois o x nele representa apenas seu início
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

    InitWindow(screenWidth, screenHeight, "raylib [core] example - input keys");

    Texture2D princesa = LoadTexture("imagens/princesa.png");
    Texture2D fundo = LoadTexture("imagens/fundo.png");

    Player player = {0};
    player.position = (Vector2){400,200};
    player.speed = 0;
    player.canJump = false;

    EnvItem envItems[] = {
        //{{x,y,largura,altura}}
        {{0,577,screenWidth,300},true,GRAY}, // chão
        {{929,412,144,38},true, RED} // plataforma
    };

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

    Vector2 PrincessPosition = player.position;

    SetTargetFPS(60);  
   

    // Main game loop
    while (!WindowShouldClose())   
    {   

        //para pegar posições no mapa:
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        printf("X: %.0f Y: %.0f\n", mouse.x, mouse.y);
        }
        

        float delta = GetFrameTime(); //GetFrameTime = tempo que passou de um frame e outro, assim garantismo que o jogo não dependa do FPS
        UpdatePlayer(&player, envItems,envItemsLength,delta);
       

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // desenhar cenário
        for (int i = 0; i < envItemsLength; i++){

            DrawRectangleRec(envItems[i].rect, envItems[i].color);
        }

        //DrawTexturePro(nome_imagem, source(qual parte da imagem eu quero usar), dest(onde e com qual tamanho desenhar na tela), origem, rotation, color);
        DrawTexturePro(
                fundo,
                (Rectangle){0, 0, fundo.width, fundo.height},//pega a imagem inteira
                (Rectangle){0, 0, screenWidth, screenHeight},
                (Vector2){0, 0},
                0.0f,
                WHITE
        );


        DrawTexturePro(
                princesa,
                (Rectangle){0, 0, princesa.width, princesa.height},
                (Rectangle){player.position.x, player.position.y, 100, 100},
                (Vector2){50, 50},
                0.0f,
                WHITE
        );


        EndDrawing();
        
    }

    UnloadTexture(fundo);
    UnloadTexture(princesa);

    CloseWindow();


    return 0;
}