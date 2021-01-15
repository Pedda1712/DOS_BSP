#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <sys/timeb.h>
#include <math.h>
#include <string.h>

#include "BSP.H"
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

	installKeyDriver(); // install the keyboard driver

	Wall firstWall = initWall({-1000 ,1000 },{1000 ,-1000 });

	BSP_Node* root = BSP_allocateNode();
	root->data = firstWall;

	int numWalls = 7;
	printf("Generiere Baum ... \n");
	printf("- aus %i Waenden\n",numWalls+1);
	for(int i = 0; i < numWalls; i++){
		BSP_insertWall(root,initWall({rand()%10000 - 5000,rand()%10000 - 5000},{rand()%10000 - 5000,rand()%10000 - 5000}));
	}
	printf("- mit %i Knoten\n",BSP_countNodes(root));
	printf("- mit Tiefe %i\n",BSP_countDepth(root));

	while(!checkKeyState(0x11)){}

	Wall* clipWalls = (Wall*)malloc(sizeof(Wall)*3);

	clipWalls[0] = { //right edge of screen
	    {0,0},
	    {-253*100,256*100}
	};
	clipWalls[1] = { //left edge of screen
	    	{255*100,256*100},
		{0,0}
	};
	clipWalls[2] = { // near Plane
	    	{ 10000,(1 << (8))},
		{ -10000,(1 << (8))}
	};

	BSP_setClipWalls(clipWalls);
	vec2D player = {0,0};
	BSP_setPlayerPosition(&player);
	mat2 testRotMat;
	BSP_setPlayerRotation(&testRotMat);

	vec2D starting_direction = {0,32};
	vec2D player_direction;
	int player_angle = 0;

	initVGA(); //set vga mode and enable nearptrs
	fillTrigTables();

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

      	//clear the drawing buffer
      	clearBuf();

		//init the rotation matrix for the camera view
		initRotationMatrix(&testRotMat,player_angle);
		player_direction = starting_direction;
		rotateVec2D(&player_direction,&testRotMat);

		BSP_drawTree(root);

		wait_for_retrace(); //VSync
		blitToScreen();
	}

	uninstallKeyDriver();
	initTEXT();

	BSP_freeTree(root);
	free(clipWalls);

	return 0;
}
