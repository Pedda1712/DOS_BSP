#pragma once
#include "MATH.H"

typedef struct Wall {
    vec2D a,b;
} Wall;

bool clipWallAgainstPlane (Wall* line,Wall* plane);

void drawWall (Wall* w); // expects wall coordinates in fixed point fractional numbers
void rotateWall(Wall* w,mat2* mat);
void moveWall(Wall* w, vec2D vec);
