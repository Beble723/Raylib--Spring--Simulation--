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

constexpr int SLDER_SPRING = 0;
constexpr int SLDER_MASS = 1;
constexpr int SLDER_DAMPING = 2;
constexpr int SLDER_DISPLACE = 3;

struct Slider
{

    const char *name; // name/label of slider
    float min;        // min value range
    float max;        // max value range
    float value;      // current value displayed/used
    float x, y, z;    // position

    // struct members
    bool Update(Vector2 mouse, bool mouseDown)
    {
        float thumbX = x + (value - min) / (max - min) * z;
        Rectangle thumb = {thumbX - 8, (float)y - 10, 16, 20};

        if (mouseDown && CheckCollisionPointRec(mouse, thumb))
        {
            float newX = mouse.x;
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

    float pos = 100.0f;
    float vel = 0.0f;
    bool isPaused = false;

    std::array<Slider, 4> sliders = {{
        {"Spring", 10, 300, 80, SLIDER_X, SLIDER_FIRST_Y, SLIDER_W},
        {"Mass", 0.5, 10, 2, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP, SLIDER_W},
        {"Damping", 0, 5, 0.3, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 2, SLIDER_W},
        {"Displace", 10, 150, 100, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 3, SLIDER_W},
    }};

    const float anchorX = 60.0f;
    const float trackY = SCREEN_H / 2.0f - 40.0f;

    const float barX = 620, barY = 42, barMaxW = 20, barH = 14;
    const float E = anchorX + 12.0f;

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        for (auto &s : sliders)
        {
            if (s.Update(mouse, mouseDown))
            {
                pos = sliders[3].value;
                vel = 0;
            }
        }

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

            if (IsKeyPressed(KEY_R))
            {
                pos = sliders[3].value;
                vel = 0;
            }
            float k1 = sliders[1].value;
            float m1 = sliders[2].value;
            float period = 2.0f * PI * std::sqrt(m / k); // std::sqrt from <cmath>
            float ke = 0.5f * m * vel * vel;
            float pe = 0.5f * k * pos * pos;
            float totalE = ke + pe;
            if (totalE < 0.001f)
                totalE = 0.001f;

            float massScreenX = E + pos;

            BeginDrawing();
            ClearBackground(BLACK);

   // Dashed equilibrium line
        for (int dx = (int)anchorX; dx < SCREEN_W - 20; dx += 14)
            DrawLine(dx, (int)trackY, dx + 7, (int)trackY, {255, 255, 255, 30});

        DrawLine((int)E, (int)trackY - 30, (int)E, (int)trackY + 30, {255,255,255,40});
        DrawText("eq", (int)E - 8, (int)trackY + 35, 11, {255,255,255,80});

        // Wall
        DrawRectangle(0, (int)trackY - 70, 18, 140, {90, 90, 95, 255});
        for (int hy = (int)trackY - 70; hy < (int)trackY + 70; hy += 13)
            DrawLine(0, hy, 18, hy + 10, {60, 60, 65, 255});
        DrawCircle((int)anchorX, (int)trackY, 5, {180, 180, 185, 255});

        // Spring
        float massLeft = massScreenX - 28;
        DrawSpring(anchorX, trackY, massLeft, COILS, COIL_HEIGHT, {127, 119, 221, 255});

        // Mass block
        int bw = 56, bh = 56;
        Rectangle massRect = {
            massScreenX - bw / 2.0f,
            trackY - bh / 2.0f,
            (float)bw, (float)bh
        };
        DrawRectangleRounded(massRect, 0.18f, 8, {15, 110, 86, 255});
        DrawRectangleRoundedLines(massRect, 0.18f, 8, {255, 255, 255, 40});
        DrawText("m",
            (int)massScreenX - MeasureText("m", 18) / 2,
            (int)trackY - 9, 18, WHITE);

        // Displacement arrow
        if (std::fabs(pos) > 4.0f) {
            float ay  = trackY + 42;
            int   dir = (pos > 0) ? 1 : -1;
            DrawLineEx({E, ay}, {massScreenX, ay}, 1.5f, {239, 159, 39, 200});
            DrawTriangle(
                {massScreenX, ay},
                {massScreenX - dir * 10.0f, ay - 5},
                {massScreenX - dir * 10.0f, ay + 5},
                {239, 159, 39, 200});

            char xbuf[32];
            snprintf(xbuf, sizeof(xbuf), "x = %.1f", pos);
            float midX = (E + massScreenX) / 2.0f;
            DrawText(xbuf,
                (int)midX - MeasureText(xbuf, 12) / 2,
                (int)ay - 16, 12, {239, 159, 39, 200});
        }

  DrawText("Controls",SLIDER_X - 10 - MeasureText("Controls", 14),
            SLIDER_FIRST_Y - 30, 14, LIGHTGRAY);



   if (isPaused) {
            DrawText("PAUSED",
                SCREEN_W / 2 - MeasureText("PAUSED", 28) / 2,
                SCREEN_H / 2 - 14, 28, {239, 159, 39, 200});
        }

            EndDrawing();
        }
CloseWindow();
        return 0;
    }
}