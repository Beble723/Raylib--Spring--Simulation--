#include <raylib.h>
#include <iostream>


int player_score = 0;
int cpu_score = 0;

Color Darkblue = {0, 128, 255, 255};   
Color Lightblue = {51, 153, 255, 255}; 
Color blue = {102, 176, 255, 255};     
Color red = {225, 102, 102, 255};      
Color white = {213, 206, 222, 250};    

class Ball
{

public:
    float x, y;
    int speedx, speedy;
    int radius;

    void draw()
    {
        DrawCircle(x, y, radius, red);
        //        DrawCircle(x, y, radius, WHITE);
    }

    void update()
    {

        x += speedx;
        y += speedy;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speedy *= -1;
        }
        if (x + radius >= GetScreenWidth())
        {
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0)
        {
            player_score++;
            ResetBall();
        }
    }
    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choice[2] = {-1, 1};
        speedx *= speed_choice[GetRandomValue(0, 1)];
        speedy *= speed_choice[GetRandomValue(0, 1)];
    }
};

class Paddles
{
protected:
    void LimitMovement()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.4, 0, WHITE); //
        // DrawRectangle(x,y,width,height,WHITE);    //
    }
    void Update()
    {

        if (IsKeyDown(KEY_W))
        {
            y = y - speed;
        }
        if (IsKeyDown(KEY_S))
        {
            y = y + speed;
        }

    if (IsKeyDown(KEY_W) && IsKeyDown(KEY_LEFT_SHIFT) )
        {
            y = y - speed-1;        // SPRINT OPTIONAL == MANY  ISSUES but memes
        }
        if (IsKeyDown(KEY_S)&& IsKeyDown(KEY_LEFT_SHIFT))
        {
            y = y + speed+1;
        }

    
        LimitMovement();
    }
};

class CPU : public Paddles
{

public:
    void Update(int ball_y)
    {

        if (y + height / 2 > ball_y)
        {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y)
        {
            y = y + speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddles player;
CPU CPUpaddle;

int main()
{
    constexpr int SCREEN_HEIGHT = 1250;
    constexpr int SCREEN_WIDTH = 800;

    ball.radius = 20;
    ball.x = SCREEN_HEIGHT / 2;
    ball.y = SCREEN_WIDTH / 2;
    ball.speedx = 7;
    ball.speedy = 7;

    player.width = 25;
    player.height = 120;
    player.x = SCREEN_HEIGHT - player.width - 10;
    player.y = SCREEN_WIDTH / 2 - player.height / 2;
    player.speed = 6;

    CPUpaddle.width = 25;
    CPUpaddle.height = 120;
    CPUpaddle.x = 10;
    CPUpaddle.y = SCREEN_WIDTH / 2 - CPUpaddle.height / 2;
    CPUpaddle.speed = 5.5;

    InitWindow(SCREEN_HEIGHT, SCREEN_WIDTH, "Pong game");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        BeginDrawing();

        // updating
        ball.update();
        player.Update();
        CPUpaddle.Update(ball.y);

        // checking for collisions
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius+1, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speedx *= -1;
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius+1, Rectangle{CPUpaddle.x, CPUpaddle.y, CPUpaddle.width, CPUpaddle.height}))
        {
            ball.speedx *= -1;
        }

        // Drawing
        ClearBackground(Lightblue); //
        // ClearBackground(BLACK);//

        DrawRectangle(SCREEN_HEIGHT / 2, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, Darkblue); //
        DrawCircle(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, 150, blue);                    //
        DrawLine(SCREEN_HEIGHT / 2, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, white);
        ball.draw();
        player.draw();
        CPUpaddle.draw();

        DrawText(TextFormat("%i", cpu_score), SCREEN_HEIGHT / 4 - 20, 20, 80, white),
            DrawText(TextFormat("%i", player_score), 3 * SCREEN_HEIGHT / 4 - 20, 20, 80, white),

            EndDrawing();
    }

    CloseWindow();
}