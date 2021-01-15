#include "WALL.H"
#include "GFX.H"
#include "MATH.H"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Wall initWall (vec2D a,vec2D b){
	vec2D w = {(b.x-a.x),(b.y-a.y)};
	char col = rand()%24 + 32;
	fp ln = sqrt(w.x*w.x + w.y*w.y);
	return {a,b,{(w.y<<F_PREC)/ln,(-w.x<<F_PREC)/ln},col};
}

void transformAndDrawWall (Wall w,Wall* cWalls, vec2D player, mat2* player_d){
	moveWall(&w,player);
	rotateWall(&w,player_d);

	bool draw = clipWallAgainstPlane(&w,&cWalls[0]);;
	if(draw)
		draw = clipWallAgainstPlane(&w,&cWalls[1]);
	if(draw)
		draw = clipWallAgainstPlane(&w,&cWalls[2]);
	if(draw)
		drawWall(&w);

}

void drawWall(Wall* w){ //transforms a wall into two 3D-triangles and draws them
    fp z1,z2;
    z1 = w->a.y;
    z2 = w->b.y;

    vec2D a1,a2; // construct edge 1 out of vertex a
    a1.y = ((900)  << (F_PREC)) / z1;
    a1.x = (w->a.x <<  F_PREC)  / z1;
    a2.x = a1.x;

    vec2D b1,b2; // construct edge 2 out of vertex b
    b1.y = ((900)  << (F_PREC)) / z2;
    b1.x = (w->b.x <<  F_PREC)  / z2;
    b2.x = a1.x;

    //transform the coordinates into screen space
    a1.x *= 160;a1.x += 160 << (F_PREC);a2.x = a1.x;
    b1.x *= 160;b1.x += 160 << (F_PREC);b2.x = b1.x;

    a1.y *= 100;a2.y = -a1.y;a1.y += 100 << F_PREC;a2.y += 100 << F_PREC;
    b1.y *= 100;b2.y = -b1.y;b1.y += 100 << F_PREC;b2.y += 100 << F_PREC;

    // discard the fractional part of the coordinates (turn them into integers)
    a1.x >>= F_PREC ;a1.y >>= F_PREC ;
    a2.x >>= F_PREC ;a2.y >>= F_PREC ;
    b1.x >>= F_PREC ;b1.y >>= F_PREC ;
    b2.x >>= F_PREC ;b2.y >>= F_PREC ;

    /*printf("%i %i\n",a1.x ,a1.y);
    printf("%i %i\n",a2.x ,a2.y);
    printf("%i %i\n",b1.x ,b1.y);
    printf("%i %i\n",b2.x ,b2.y);*/

    // draw two triangles to represent the wall
    ucTriangle(a1,a2,b1,w->col);
    ucTriangle(b1,b2,a2,w->col);
}

void rotateWall(Wall* w,mat2* mat){
    rotateVec2D(&w->a,mat);
    rotateVec2D(&w->b,mat);
}
void moveWall(Wall* w, vec2D vec){
    w->a.x += vec.x;
    w->a.y += vec.y;
    w->b.x += vec.x;
    w->b.y += vec.y;
}


bool clipWallAgainstPlane (Wall* line,Wall* plane){
	vec2D p1 = line->a;
	vec2D p2 = line->b;
	vec2D p3 = plane->a;
	vec2D p4 = plane->b;

	vec2D planeVec = {p3.x - p4.x,p3.y - p4.y};
	vec2D planeNorm = {planeVec.y,-planeVec.x};

	vec2D lineToPlaneVec1 = {p3.x-p1.x,p3.y-p1.y};
	vec2D lineToPlaneVec2 = {p3.x-p2.x,p3.y-p2.y};
	//Calculate Dot Product between Wall Normal and Plane Normal
	fp dot1 = (planeNorm.x * lineToPlaneVec1.x + planeNorm.y * lineToPlaneVec1.y);
	fp dot2 = (planeNorm.x * lineToPlaneVec2.x + planeNorm.y * lineToPlaneVec2.y);

	if((dot1 < 0 && dot2 < 0)){ // If both Points are not on the correct side
		return false; //false -> the entire wall is invisible
	}else if((dot1 > 0 && dot2 > 0)){ // If both Points are on the correct side
		return true;
	}

	/*
		Calculate the Intersection Point (See Wikipedia "Line-Line Intersection")
	*/

	fp divisor = ((p1.x - p2.x)*(p3.y - p4.y) - (p1.y-p2.y)*(p3.x-p4.x)) >> F_PREC;

	fp t = ((p1.x-p3.x)*(p3.y-p4.y)-(p1.y-p3.y)*(p3.x-p4.x))/divisor;

	/*
		NOTE: We do not need to check if the Wall even intersects the plane (divisor == 0?), that is
		already accuonted for with the Dot Product calculation earlier in the method
	*/

	vec2D intersection;
	intersection.x = p1.x + ((t*(p2.x-p1.x))>>F_PREC);
	intersection.y = p1.y + ((t*(p2.y-p1.y))>>F_PREC);

	if(dot1 > 0){ //If Point p1 is on the correct side
		line->b = intersection; //assign the intersection point to p2
	}else if(dot2 > 0){ // If Point p2 is on the correct side
		line->a = intersection; //assign the intersection point to p1
	}

	return true; // true -> draw the wall
}

bool sliceWallWithPlane (Wall* line,Wall* line2,Wall* plane){
	vec2D p1 = line->a;
	vec2D p2 = line->b;
	vec2D p3 = plane->a;
	vec2D p4 = plane->b;

	vec2D planeVec = {p3.x - p4.x,p3.y - p4.y};
	vec2D planeNorm = {planeVec.y,-planeVec.x};

	vec2D lineToPlaneVec1 = {p3.x-p1.x,p3.y-p1.y};
	vec2D lineToPlaneVec2 = {p3.x-p2.x,p3.y-p2.y};
	//Calculate Dot Product between Wall Normal and PlayerToWall Normal
	fp dot1 = (planeNorm.x * lineToPlaneVec1.x + planeNorm.y * lineToPlaneVec1.y);
	fp dot2 = (planeNorm.x * lineToPlaneVec2.x + planeNorm.y * lineToPlaneVec2.y);

	if((dot1 < 0 && dot2 < 0)){ // If both Points are not on the correct side
		return false; //false, the wall is not sliceable by plane
	}else if((dot1 > 0 && dot2 > 0)){ // If both Points are on the correct side
		return false; // false, the wall is not sliceable by plane
	}

	/*
		Calculate the Intersection Point (See Wikipedia "Line-Line Intersection")
	*/

	fp divisor = ((p1.x - p2.x)*(p3.y - p4.y) - (p1.y-p2.y)*(p3.x-p4.x)) >> F_PREC;

	if( divisor == 0) return false;

	fp t = ((p1.x-p3.x)*(p3.y-p4.y)-(p1.y-p3.y)*(p3.x-p4.x))/divisor;

	vec2D intersection;
	intersection.x = p1.x + ((t*(p2.x-p1.x))>>F_PREC);
	intersection.y = p1.y + ((t*(p2.y-p1.y))>>F_PREC);

	// Slice the Wall
	line->b = intersection; //assign the intersection point to p2
	line2->a = intersection; //assign the intersection point to p1

	return true; // true -> slice the wall
}
