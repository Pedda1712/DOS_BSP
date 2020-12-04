#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <sys/timeb.h>
#include <math.h>
#include <string.h>

#include "MATH.H"
#include "GFX.H"
#include "KEY_INPUT.H"
#include "WALL.H"

#define INPUT_STATUS 0x3DA
#define VRETRACE_BIT 0x08

/* when leaving this function , the monitor is in retrace*/
void wait_for_retrace(){
	while (inp(INPUT_STATUS) & VRETRACE_BIT);
	while (!(inp(INPUT_STATUS) & VRETRACE_BIT));
}

int main () {

	initVGA(); //set vga mode and enable nearptrs
	installKeyDriver(); // install the keyboard driver
	fillTrigTables();

	Wall testWall = { // 1 = 1 /(F_PREC^2) (1/256 for F_PREC = 8)
	    {512,1512},
	    {-512,1512}
    	};
	Wall transformedWall;
	mat2 testRotMat;

	Wall clipWall = { //right edge of screen
	    {0,0},
	    {-253*100,256*100}
	};
	Wall clipWall2 = { //left edge of screen
	    	{255*100,256*100},
		{0,0}
	};
	Wall clipWall3 = { // near Plane
	    	{ 10000,(1 << (8))},
		{ -10000,(1 << (8))}
	};

	vec2D player = {0,0};
	vec2D starting_direction = {0,32};
	vec2D player_direction;
	int player_angle = 0;

	bool running = true;

	while(running){

      	if(checkKeyState(0x01)){ //check if Escape-Key was pressed
      		running = false;
      	}

		if(checkKeyState(0x11)){
			player.y -= player_direction.y;
			player.x += player_direction.x;
		}
		if(checkKeyState(0x1f)){
			player.y += player_direction.y;
			player.x -= player_direction.x;
		}
		if(checkKeyState(0x1e)){
			player_angle -= 1 << (F_PREC-7);
		}
		if(checkKeyState(0x20)){
			player_angle += 1 << (F_PREC-7);
		}

      	wait_for_retrace(); //VSync
      	blitToScreen();

      	//clear the drawing buffer
      	clearBuf();

		//init the rotation matrix for the camera view
		initRotationMatrix(&testRotMat,player_angle);
		player_direction = starting_direction;
		rotateVec2D(&player_direction,&testRotMat);

      	// rotate and move the wall, then draw it
		transformedWall = testWall;
      	moveWall(&transformedWall,player);
		rotateWall(&transformedWall,&testRotMat);

		bool draw = clipWallAgainstPlane(&transformedWall,&clipWall3);;
		if(draw)
			draw = clipWallAgainstPlane(&transformedWall,&clipWall2);
		if(draw)
	  		draw = clipWallAgainstPlane(&transformedWall,&clipWall);
		if(draw)
        		drawWall(&transformedWall);
	}

	uninstallKeyDriver();
	initTEXT();

	return 0;
}
