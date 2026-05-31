#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <cstdio>
#include <array>

// window

constexpr int SCREEN_W = 900;
constexpr int SCREEN_H = 600;

constexpr int COILS = 14;
constexpr int COIL_HEIGHT = 18;

constexpr int SLIDER_X = 60;
constexpr int SLIDER_W = 220;
constexpr int SLIDER_FIRST_Y = 420;
constexpr int SLIDER_GAP = 42;

constexpr int MAX = 300;

struct Slider
{

    const char *name; // name/label of slider--- pointer to show whole name
    float min;        // min value range
    float max;        // max value range
    float value;      // current value displayed/used
    float x, y, w;    // position

    // struct members
    bool Update(Vector2 mouse, bool mouseDown) {
        float thumbX = x + (value - min) / (max - min) * w;
        Rectangle thumb = { thumbX - 8, (float)y - 10, 16, 20 };

        if (mouseDown && CheckCollisionPointRec(mouse, thumb)) {
            float newX = mouse.x;
            if (newX < x)     newX = (float)x;
            if (newX > x + w) newX = (float)(x + w);
            value = min + (newX - x) / w * (max - min);
            return true;
        }
        return false;
    }

    void Draw() const
    {
        float thumbX = x + (value - min) / (max - min) * w;

        DrawLineEx({(float)x, (float)y}, {(float)(x + w), (float)y}, 2, LIGHTGRAY);
        DrawLineEx({(float)x, (float)y}, {thumbX, (float)y}, 3, {83, 74, 183, 255});

        DrawCircle((int)thumbX, y, 8, {83, 74, 183, 255});
        DrawCircleLines((int)thumbX, y, 8, WHITE);

        DrawText(name, x - 10 - MeasureText(name, 13), y - 6, 13, LIGHTGRAY);

        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", value);
        DrawText(buf, x + w + 10, y - 6, 13, WHITE);
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

// main
int main()
{

    InitWindow(SCREEN_W, SCREEN_H, "Spring:Simulation");
    SetTargetFPS(60);

    float pos = 100.0f;
    float vel = 0.0f;
    bool isPaused = false;

    std::array<float, MAX> tx{};
    std::array<float, MAX> tv{};
    int th = 0;
    int tc = 0;

    std::array<Slider, 4> sliders = {{
        {"Spring", 10, 300, 80, SLIDER_X, SLIDER_FIRST_Y, SLIDER_W},
        {"Mass", 0.5, 10, 2, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP, SLIDER_W},
        {"Damping", 0, 5, 0.3, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 2, SLIDER_W},
        {"Displace", 10, 150, 100, SLIDER_X, SLIDER_FIRST_Y + SLIDER_GAP * 3, SLIDER_W},
    }};

    // ^^ ARRAY indexes ?
    constexpr int K1 = 0;
    constexpr int M1 = 1;
    constexpr int D1 = 2;
    constexpr int D2 = 3;

    const float anchorX = 60.0f;
    const float trackY = SCREEN_H / 2.0f - 40.0f;
  const float E = anchorX + 120.0f;

    const float barX = 620, barY = 420, barMaxW = 200, barH = 14;
  

    const float px = 620, py = 80, pw = 200, ph = 160;

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
                th = 0;
                tc = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE))
            isPaused = !isPaused;

        if (IsKeyPressed(KEY_R))
        {
            pos = sliders[3].value;
            vel = 0;
            th = 0;
            tc = 0;
        }

        if (!isPaused)
        {
            float k = sliders[0].value;
            float m = sliders[1].value;
            float b = sliders[2].value;
          

            float accel = (-k * pos - b * vel) / m;
            vel += accel * dt;
            pos += vel * dt;

            tx[th] = pos;
            tv[th] = vel;
            th = (th + 1) % MAX;
            if (tc < MAX)
                tc++;
        }
        float k1 = sliders[0].value;
        float m1 = sliders[1].value;
        float period = 2.0f * PI * std::sqrt(m1 / k1); // std::sqrt from <cmath>
        float ke = 0.5f * m1 * vel * vel;
        float pe = 0.5f * k1 * pos * pos;
        float totalE = ke + pe;
        if (totalE < 0.001f)
            totalE = 0.001f;

        float massScreenX = E + pos;

        // DRRASWING

        BeginDrawing();
        ClearBackground(BLACK);

        // Dashed equilibrium line
        for (int dx = (int)anchorX; dx < SCREEN_W - 20; dx += 14)
            DrawLine(dx, (int)trackY, dx + 7, (int)trackY, {255, 255, 255, 30});

        DrawLine((int)E, (int)trackY - 30, (int)E, (int)trackY + 30, {255, 255, 255, 40});
        DrawText("eq", (int)E - 8, (int)trackY + 35, 11, {255, 255, 255, 80});

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
            (float)bw, (float)bh};

        DrawRectangleRounded(massRect, 0.18f, 8, {15, 110, 86, 255});
        DrawRectangleRoundedLines(massRect, 0.18f, 8, {255, 255, 255, 40});
        DrawText("m",
                 (int)massScreenX - MeasureText("m", 18) / 2,
                 (int)trackY - 9, 18, WHITE);

        // Displacement arrow
        if (std::fabs(pos) > 4.0f)
        {
            float ay = trackY + 42;
            int dir = (pos > 0) ? 1 : -1;
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

        // pid diragram/graph

        DrawRectangleLines((int)px, (int)py, (int)pw, (int)ph, {255, 255, 255, 40});
        DrawText("digaram", (int)px, (int)(py + ph + 6), 11, {255, 255, 255, 100});

        float psCx = px + pw / 2, psCy = py + ph / 2;
        DrawLine((int)px, (int)psCy, (int)(px + pw), (int)psCy, {255, 255, 255, 30});
        DrawLine((int)psCx, (int)py, (int)psCx, (int)(py + ph), {255, 255, 255, 30});

        float maxPos = sliders[3].value + 1.0f;
        float maxVel = maxPos * std::sqrt(k1 / m1) + 1.0f;
        for (int i = 0; i < tc; i++)
        {
            int idx = (th - 1 - i + MAX) % MAX;
            float px = psCx + tx[idx] / maxPos * (pw / 2 - 4);
            float py = psCy - tv[idx] / maxVel * (ph / 2 - 4);
            auto alpha = (unsigned char)(180 * (1.0f - (float)i / tc));
            DrawPixel((int)px, (int)py, {239, 159, 39, alpha});
        }

        // Energy bar graph
        DrawText("Energy", (int)barX, (int)barY - 22, 13, LIGHTGRAY);

        float peW = (pe / totalE) * barMaxW;
        DrawRectangleRounded({barX, barY, peW, barH}, 0.5f, 4, {226, 75, 74, 220});
        DrawText("PE", (int)(barX + peW + 6), (int)barY, 12, {226, 75, 74, 220});

        float keW = (ke / totalE) * barMaxW;
        DrawRectangleRounded({barX, barY + 20, keW, barH}, 0.5f, 4, {55, 138, 221, 220});
        DrawText("KE", (int)(barX + keW + 6), (int)(barY + 20), 12, {55, 138, 221, 220});

        char buf[64];
        float statsX = 620, statsY = 290;

        DrawText("Stats", (int)statsX, (int)statsY, 14, LIGHTGRAY);

        snprintf(buf, sizeof(buf), "Position:  %.2f px", pos);
        DrawText(buf, (int)statsX, (int)statsY + 22, 13, WHITE);

        snprintf(buf, sizeof(buf), "Velocity:  %.2f px/s", vel);
        DrawText(buf, (int)statsX, (int)statsY + 40, 13, WHITE);

        snprintf(buf, sizeof(buf), "Time:  %.2f s", period);
        DrawText(buf, (int)statsX, (int)statsY + 58, 13, WHITE);

        snprintf(buf, sizeof(buf), "KE: %.1f  PE: %.1f", ke, pe);
        DrawText(buf, (int)statsX, (int)statsY + 76, 13, {180, 180, 185, 255});

        DrawText("Controls", SLIDER_X - 10 - MeasureText("Controls", 14),
                 SLIDER_FIRST_Y - 30, 14, LIGHTGRAY);
        for (const auto &s : sliders)
            s.Draw();

        if (isPaused)
        {
            DrawText("PAUSED",
                     SCREEN_W / 2 - MeasureText("PAUSED", 28) / 2,
                     SCREEN_H / 2 - 14, 28, {239, 159, 39, 200});
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
