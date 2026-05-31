#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <iostream> // do i need this too?
#include <cstdio>   // do i need this?
#include <array>
// future me add coments pls im beging you
// window

constexpr int SCREEN_W = 900;
constexpr int SCREEN_H = 600;

constexpr int COILS = 18;
constexpr int COIL_HEIGHT = 18;

constexpr int SLIDER_X = 60;
constexpr int SLIDER_W = 220;
constexpr int SLIDER_FIRST_Y = 420;
constexpr int SLIDER_GAP = 42;

constexpr int SLDER_SPRING  = 0;
constexpr int SLDER_MASS    = 1;
constexpr int SLDER_DAMPING = 2;
constexpr int SLDER_DISPLACE = 3;

struct Slider
{

    const char* name; // name/lable of slider
    float min;       // min value range
    float max;       // max value range
    float value;     // current value displayed/used
    float x, y, z;   // position

    // struct members
    bool Update(Vector2 mouse, bool mouseDown)
    {
        float thumbX = x + (value - min) / (max - min) * z;
        Rectangle thumb = {thumbX - 8, (float)y - 10, 16, 20};

        if (mouseDown && CheckCollisionPointRec(mouse, thumb))
        {
            float newX = mouse.x;
            if (newX < x)
                newX = (float)x;
            if (newX > x + z)
                newX = (float)(x + z);
            value = min + (newX - x) / z * (max - min);
            return true;
        }
        return false;
    }
    void Draw() const
    {
        float thumbX = x + (value - min) / (max - min) * z;

        DrawLineEx({(float)x, (float)y}, {(float)(x + z), (float)y}, 2, LIGHTGRAY);
        DrawLineEx({(float)x, (float)y}, {thumbX, (float)y}, 3, {83, 74, 183, 255});

        DrawCircle((int)thumbX, y, 8, {83, 74, 183, 255});
        DrawCircleLines((int)thumbX, y, 8, WHITE);

        DrawText(name, x - 10 - MeasureText(name, 13), y - 6, 13, LIGHTGRAY);

        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", value);
        DrawText(buf, x + z + 10, y - 6, 13, WHITE);
    }

  
};

  void DrawSpring(float x1, float y, float x2, int coils, float height, Color col)
    {
        float len = x2 - x1;
        float segW = len / (coils * 2 + 2);

        Vector2 prev = {x1, y};
        Vector2 p1 = {x1 + segW, y};
        DrawLineEx(prev, p1, 2, col);
        prev = p1;

        for (int i = 0; i < coils; i++)
        {
            float bx = x1 + segW + i * 2 * segW;
            Vector2 top = {bx + segW * 0.5f, y - height};
            Vector2 bot = {bx + segW * 1.5f, y + height};
            DrawLineEx(prev, top, 2, col);
            DrawLineEx(top, bot, 2, col);
            prev = bot;
        }

        Vector2 end = {x2, y};
        DrawLineEx(prev, end, 2, col);
    }


int main()
{

    InitWindow(SCREEN_W, SCREEN_H, "Spring:Simulation");
    SetTargetFPS(60);

    float pos= 100.0f;
    float vel= 0.0f;
    bool isPaused = false;

  

    std::array<Slider, 4> sliders = {{
        {"Spring", 10, 300, 80, SLIDER_X, SLIDER_FIRST_Y, SLIDER_W},
        {"Mass", 0.5, 10, 2, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP, SLIDER_W},
        {"Damping", 0, 5, 0.3, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 2, SLIDER_W},
        {"Displace", 10, 150, 100, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 3, SLIDER_W},
    }};

    while (!WindowShouldClose())
    {

          float dt = GetFrameTime();
    Vector2 mouse = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (IsKeyPressed(KEY_SPACE))
            isPaused = !isPaused;

        if (!isPaused)
        {
            float k = sliders[0].value;
            float m = sliders[1].value;
            float b = sliders[2].value;
            float d = sliders[3].value;

            float accel = (-k * pos - b * vel) / m;
            vel += accel * dt;
            pos += vel * dt;

            BeginDrawing();
            ClearBackground(BLACK);

            EndDrawing();
        }
      

        return 0;
    }
}