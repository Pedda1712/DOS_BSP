#pragma once

#include "MATH.H"

void setMode(unsigned char mode);
void initVGA();
void initTEXT();

void clearBuf();
void blitToScreen();

void bTriangle(vec2D v1 , vec2D v2, vec2D v3, byte col);
void tTriangle(vec2D v1 , vec2D v2, vec2D v3, byte col);
void ucTriangle(vec2D v1 , vec2D v2, vec2D v3, byte col);
