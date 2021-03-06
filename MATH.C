#include "MATH.H"
#include <math.h>
#include <stdio.h>

fp SIN[TRIG_RESOLUTION];
fp COS[TRIG_RESOLUTION];

void fillTrigTables(){
    for(int i = 0; i < TRIG_RESOLUTION; ++i){
        SIN[i] = (1 << (F_PREC)) * sin( ((float)i/(float)TRIG_RESOLUTION) * 6.283185f);
        COS[i] = (1 << (F_PREC)) * cos( ((float)i/(float)TRIG_RESOLUTION) * 6.283185f);
    }
}

void initRotationMatrix (mat2* mat, fp angle){ // 1 << F_PREC == 2 * Pi
    fp index = ((angle * TRIG_RESOLUTION) >> F_PREC) % TRIG_RESOLUTION;

    if(index < 0){
        index = (TRIG_RESOLUTION + index);
    }

    mat->mat[0] = COS[index];
    mat->mat[1] = -SIN[index];

    mat->mat[2] = SIN[index];
    mat->mat[3] = COS[index];

}

void rotateVec2D (vec2D* vec,mat2* mat){
	fp x = vec->x;
	fp y = vec->y;
	//printf("%i,%i\n",x,y);
	// values of vec and mat are both in fractional space, the result ends up in double fractional space so we have to >> F_PREC
	//printf("%i,%i\n",mat->mat[0],mat->mat[1]);
	vec->x = ((x) * mat->mat[0] + (y) * mat->mat[1]) >> F_PREC;
	vec->y = ((x) * mat->mat[2] + (y) * mat->mat[3]) >> F_PREC;
}
