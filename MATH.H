#pragma once

//resoultion of the Sine/Cosine Tables
#define TRIG_RESOLUTION 1024
/* Fixed Point Math*/
#define F_PREC 12 /* number of bits that represent the fractional part*/
typedef int fp;

typedef unsigned char byte; /* define unsigned char as a byte */

/* 2D Vector struct */
typedef struct vec2D {fp x,y;} vec2D;
/* 2x2 Matrix struct*/
typedef struct mat2 {fp mat[4];} mat2;

void fillTrigTables(); // Fills the Sine/Cosine Tables

void initRotationMatrix (mat2* mat, fp angle);
void rotateVec2D (vec2D* vec,mat2* mat);
