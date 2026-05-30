#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

int main(){



//window

constexpr int SCREEN_W= 900;
constexpr int SCREEN_H= 600;


InitWindow(SCREEN_W,SCREEN_H,"Spring:Simulation");


while(!WindowShouldClose()){

BeginDrawing();








EndDrawing();
}

    return 0;
}