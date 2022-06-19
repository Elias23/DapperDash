#include "raylib.h"
#include <list>
#include <iostream>

struct graphics
{
    Rectangle rect;
    const Texture2D texture;
    int frameLimnit;
    int graphicsFrame{0};
    const float graphicsSpeed{1 / 12.0f};
    float runningTime{0.0f};
};
struct dasherChar
{
    Vector2 pos;
    struct graphics graphics;
    const int jumpVel{-600};
    int velocity{0};
    bool isJumping{false};
};
struct nebula
{
    Vector2 pos;
    struct graphics graphics;
    int velocity{-200};
    bool toBeRemoved{false};
};
struct gameData
{
    const int targetFPS;
    struct dasherChar dasherChar;
    std::list<struct nebula> nebulas;
    const int screenWidth;
    const int screenHeight;
    const Texture2D background;
    const Texture2D midground;
    const Texture2D foreground;
    float finishLine;
    float bgPos;
    float mgPos;
    float fgPos;
    const int gravity{1000};
    bool isGameOver{false};
    bool isGameWon{false};
    bool debug{true};
    // Graphics
};

bool isOnGround(struct dasherChar dasherChar, int groundLevel)
{
    return dasherChar.pos.y + dasherChar.graphics.rect.height >= groundLevel;
}
void animationUpdate(struct graphics *graphics, float dt)
{
    graphics->runningTime += dt;
    int frameCols = graphics->texture.width / graphics->rect.width;
    int frameRows = graphics->texture.height / graphics->rect.height;
    if (graphics->runningTime > graphics->graphicsSpeed)
    {
        graphics->runningTime = 0.0f;
        graphics->graphicsFrame++;
        if (graphics->graphicsFrame >= graphics->frameLimnit)
        {
            graphics->graphicsFrame = 0;
        }
    }
    graphics->rect.x = (graphics->graphicsFrame % frameCols) * graphics->rect.width;
    graphics->rect.y = (graphics->graphicsFrame / frameRows) * graphics->rect.height;
}
void update(struct gameData *game_data, const float &deltaTime)
{

    dasherChar &dasher = game_data->dasherChar;
    std::list<nebula> &nebulas = game_data->nebulas;
    if (isOnGround(dasher, game_data->screenHeight))
    {
        dasher.isJumping = false;
        dasher.velocity = 0;
    }
    else
    {
        dasher.velocity += game_data->gravity * deltaTime;
        dasher.isJumping = true;
    }
    if (IsKeyPressed(KEY_SPACE) && !dasher.isJumping)
    {
        dasher.velocity += dasher.jumpVel;
    }
    // update dasher
    dasher.pos.y += dasher.velocity * deltaTime;

    if (!nebulas.empty())
    {
        for (std::list<nebula>::iterator n = nebulas.begin(); n != nebulas.end();)
        {
            n->pos.x += n->velocity * deltaTime;
            if (n->pos.x <= -n->graphics.rect.width)
            {
                nebulas.erase(n++);
                // n->pos.x += 2 * game_data->screenWidth;
                n++;
            }
            else
            {
                float pad = 50;
                game_data->isGameOver |= CheckCollisionRecs(
                    {n->pos.x + pad, n->pos.y + pad, n->graphics.rect.width - 2 * pad, n->graphics.rect.height - 2 * pad},
                    {dasher.pos.x, dasher.pos.y, dasher.graphics.rect.width, dasher.graphics.rect.height});
                n++;
            }
        }
    }
    // update finish line
    game_data->finishLine += game_data->nebulas.back().velocity * deltaTime;
    if (game_data->finishLine < game_data->dasherChar.pos.x)
    {
        game_data->isGameWon = true;
    }
    // update background position
    game_data->bgPos += -20 * deltaTime;
    if (game_data->bgPos <= -game_data->background.width * 2)
    {
        game_data->bgPos = 0;
    }
    game_data->mgPos += -40 * deltaTime;
    if (game_data->mgPos <= -game_data->midground.width * 2)
    {
        game_data->mgPos = 0;
    }
    game_data->fgPos += -80 * deltaTime;
    if (game_data->fgPos <= -game_data->foreground.width * 2)
    {
        game_data->fgPos = 0;
    }

    // dasher animation
    if (!dasher.isJumping)
    {
        animationUpdate(&dasher.graphics, deltaTime);
    }
    // nebula animation
    for (nebula &n : nebulas)
    {
        animationUpdate(&n.graphics, deltaTime);
    }
};

void drawBackground(struct gameData *game_data)
{

    // background
    DrawTextureEx(game_data->background, {game_data->bgPos, 0}, 0.0, 2.0, WHITE);
    DrawTextureEx(game_data->background, {game_data->bgPos + game_data->background.width * 2, 0}, 0.0, 2.0, WHITE);
    // midground
    DrawTextureEx(game_data->midground, {game_data->mgPos, 0}, 0.0, 2.0, WHITE);
    DrawTextureEx(game_data->midground, {game_data->mgPos + game_data->midground.width * 2, 0}, 0.0, 2.0, WHITE);
    // foreground
    DrawTextureEx(game_data->foreground, {game_data->fgPos, 0}, 0.0, 2.0, WHITE);
    DrawTextureEx(game_data->foreground, {game_data->fgPos + game_data->foreground.width * 2, 0}, 0.0, 2.0, WHITE);
}

void draw(struct gameData *game_data, const float &deltaTime)
{
    dasherChar &dasher = game_data->dasherChar;
    std::list<nebula> &nebulas = game_data->nebulas;
    float pad = 50;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    drawBackground(game_data);
    if (game_data->isGameOver)
    {
        DrawText("GameOver!", game_data->screenWidth / 2 - MeasureText("GameOver!", 50) / 2, game_data->screenHeight / 2 - 50, 50, WHITE);
    }
    else if (game_data->isGameWon)
    {
        DrawText("You Win!", game_data->screenWidth / 2 - MeasureText("You Win!", 50) / 2, game_data->screenHeight / 2 - 50, 50, WHITE);
    }
    else
    {
        // Draw Nebula
        for (nebula n : nebulas)
        {
            if (game_data->debug)
                DrawRectangle(n.pos.x + pad, n.pos.y + pad, n.graphics.rect.width - 2 * pad, n.graphics.rect.height - 2 * pad, RED);
            DrawTextureRec(n.graphics.texture, n.graphics.rect, n.pos, WHITE);
        }
        // Draw dasher
        if (game_data->debug)
            DrawRectangle(dasher.pos.x, dasher.pos.y, dasher.graphics.rect.width, dasher.graphics.rect.height, BLUE);
        DrawTextureRec(dasher.graphics.texture, dasher.graphics.rect, dasher.pos, WHITE);
    }
    EndDrawing();
}

int main(int argc, char const *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 512;
    const int screenHeight = 350;
    const int targetFPS = 60;
    InitWindow(screenWidth, screenHeight, "Dasher");
    SetTargetFPS(targetFPS);
    // scarfy
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Texture2D nebulaTex = LoadTexture("textures/nebula.png");
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    Rectangle scarfyRect = {0, 0, scarfy.width / 6, scarfy.height};
    dasherChar dasher = {{screenWidth / 2 - scarfyRect.width / 2, screenHeight - scarfyRect.height}, {scarfyRect, scarfy, 6}};
    // nebula
    Texture2D nebulaTexture = LoadTexture("textures/12_nebula_spritesheet.png");
    Rectangle nebulaRect{0.0, 0.0, nebulaTexture.width / 8, nebulaTexture.height / 8};
    std::list<struct nebula> nebulas{};

    for (int i = 0; i < 10; i++)
    {
        nebula nebula = {{screenWidth + i * 300, screenHeight - nebulaRect.height}, {nebulaRect, nebulaTexture, 61}};
        nebulas.push_back(nebula);
    }

    // gameData
    gameData game_data{targetFPS,
                       dasher,
                       nebulas,
                       screenWidth,
                       screenHeight,
                       background,
                       midground,
                       foreground,
                       nebulas.back().pos.x};

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        update(&game_data, deltaTime);
        draw(&game_data, deltaTime);
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebulaTex);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
    return 0;
}
