#include "GFX.H"
#include <string.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <stdio.h>

byte *VGA= (byte *)0xA0000; /* pointer to screen memory */

byte buf [320 * 200]; /* offscreen memory buffer*/

/*  (y<<8)+(y<<6)) is the same as 320*y */
#define PX(x , y, c)    VGA[((y) << 8) + ((y) << 6) + (x)]=(c) /* write directly into memory */
#define PXDB(x , y , c) buf[((y) << 8) + ((y) << 6) + (x)]=(c) /*write into offscreen buffer*/

void setMode (unsigned char mode) { //Sets 'mode' as graphics mode
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
    int86(0x10, &regs , &regs);
}

void initVGA(){ //enables VGA mode
    __djgpp_nearptr_enable(); /* so we can actually adress the screen mem*/
    VGA += __djgpp_conventional_base; /*VGA mem comes right after conventional*/
    setMode(0x13); /* set VGA 320x200x256 mode */
}

void initTEXT(){ //enables text mode
    setMode(0x03); /* set mext mode */

    /* disable nearptr mode (lock memory)*/
    __djgpp_nearptr_disable();
}

void clearBuf(){ // clears the drawing buffer
    memset(buf,0,320*200);
}

void blitToScreen() { // copies the drawing buffer into screen memory
    memcpy(VGA,buf,320*200);
}

/* bTriangle : draws a bottom-flat triangle
 * v1 is the "top", v2/v3 order doesn't matter*/

void bTriangle(vec2D v1 , vec2D v2, vec2D v3, byte col){
    fp p1_x_step = ((v2.x - v1.x) << F_PREC)/(v2.y-v1.y);
    fp p2_x_step = ((v3.x - v1.x) << F_PREC)/(v3.y-v1.y);

    fp current_x1 = (v1.x << F_PREC) ;
    fp current_x2 = (v1.x << F_PREC) ;

    fp* min;
    fp* max;

    if( (v2.x - v3.x) > 0 ) {
        max = &current_x1;
        min = &current_x2;
    }else {
        max = &current_x2;
        min = &current_x1;
    }

    if(v1.y < 0){ // If the Triangle is out of (vertical) bounds
	    int diff = -v1.y;
	    v1.y = 0;
	    current_x1 += p1_x_step * diff;
	    current_x2 += p2_x_step * diff;
    }
    if(v2.y > 199){
	    v2.y = 199;
    }

    for(int i = (v1.y);i <= (v2.y);++i){
	    /*for(int k = -1; k <= ((*max)-(*min)) >> F_PREC;++k){

		    PXDB( ( (*min) >> F_PREC )+k,i,col);

	    }*/
	  memset(buf + ((*min) >> F_PREC) + (i * 320),col,(((*max)-(*min)) >> F_PREC)+1);

        PXDB(((*max) >> F_PREC),i,col);
        PXDB(((*min) >> F_PREC)-1,i,col);

        current_x1 += p1_x_step;
        current_x2 += p2_x_step;

    }
}


/* tTriangle - draws a top-flat triangle
 *v3 is the lowest point, v1/v2 order doesn't matter*/

void tTriangle (vec2D v1 , vec2D v2, vec2D v3, byte col) {
    fp p1_x_step = ((v1.x - v3.x) << F_PREC) / (v1.y - v3.y);
    fp p2_x_step = ((v2.x - v3.x) << F_PREC) / (v2.y - v3.y);

    fp current_x1 = (v3.x << F_PREC) ;
    fp current_x2 = (v3.x << F_PREC) ;

    fp* max;
    fp* min;

    if ((v1.x - v2.x) > 0) {
        max = &current_x1;
        min = &current_x2;
    }else{

        max = &current_x2;
        min = &current_x1;
    }

    if(v3.y > 199){
	    int diff = v3.y - 199;
	    v3.y = 199;
	    current_x1 -= p1_x_step * diff;
	    current_x2 -= p2_x_step * diff;
    }
    if(v1.y < 0){
	    v1.y = 0;
    }

    for (int i = v3.y; i >= v1.y; --i) {
        /*for ( int k = -1; k <= ( ((*max) - (*min)) >> F_PREC );++k ){

            PXDB(((*min) >> F_PREC)+k,i,col);

        }*/
        memset(buf + ((*min) >> F_PREC) + (i * 320),col,(((*max)-(*min)) >> F_PREC)+1);

        PXDB(((*max) >> F_PREC),i,col);
        PXDB(((*min) >> F_PREC)-1,i,col);

        current_x1 -= p1_x_step;
        current_x2 -= p2_x_step;
    }
}

void ucTriangle (vec2D v1 , vec2D v2, vec2D v3){ //draws a generic triangle
    //sort the vertices
    vec2D min,mid,max;
    if( v1.y > v2.y ) {
        max = v1;
        mid = v2;
    }else{
        max = v2;
        mid = v1;
    }
    if(v3.y <= mid.y){
        min = v3;
    }else if(v3.y <= max.y){
        min = mid;
        mid = v3;
    }else{
        min = mid;
        mid = max;
        max = v3;
    }

    //Draw the triangles(s)
    byte col = 12;
    if ( max.y == mid.y){ // if the triangle is flat on the bottom
        bTriangle(min,mid,max,col);
    }else if(min.y == mid.y){ // if the triangle is flat on the top
        tTriangle(min,mid,max,col);
    }else{ // if the triangle is neither, divide it into one bottom and one top flat triangle
        vec2D div = {min.x + ((( ((mid.y - min.y) << F_PREC) / ((max.y - min.y)) ) * (max.x - min.x)) >> F_PREC) , mid.y};
        bTriangle(min,mid,div,col);
        tTriangle(mid,div,max,col);
    }

}
