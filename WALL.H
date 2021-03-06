#pragma once
#include "MATH.H"

typedef struct Wall {
    vec2D a,b,n;
    char col;
} Wall;

Wall initWall (vec2D a,vec2D b);

bool clipWallAgainstPlane (Wall* line,Wall* plane); // returns true if part/all of the wall is on the correct side
bool sliceWallWithPlane (Wall* line,Wall* line2,Wall* plane); // returns false if the wall is not sliceable

void drawWall (Wall* w); // expects wall coordinates in fixed point fractional numbers
void transformAndDrawWall (Wall w,Wall* cWalls, vec2D player, mat2* player_d);
void rotateWall(Wall* w,mat2* mat);
void moveWall(Wall* w, vec2D vec);
