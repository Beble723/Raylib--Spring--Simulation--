#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

int main(){



//window

constexpr int SCREEN_W= 900;
constexpr int SCREEN_H= 600;
constexpr int SPRING_TURNS=40;
constexpr int SPRING_POS_X=SCREEN_W;
constexpr int SPRING_POS_Y=SCREEN_H/2;

struct main
{
    sliders x;
    sliders y
    Vector2l
};



bool isPaused =false;

if(KEY_R){
    isPaused = true;
} 
InitWindow(SCREEN_W,SCREEN_H,"Spring:Simulation");


while(!WindowShouldClose()){

BeginDrawing();





DrawText("Space = pause | R= Reset",SCREEN_W-20,SCREEN_H-20,RAYWHITE);
if(isPaused){

    DrawText("Paused",SCREEN_W-20,SCREEN_H-20,RAYWHITE);
}

EndDrawing();
}

    return 0;
}