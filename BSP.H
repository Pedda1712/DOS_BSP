#pragma once
#include "WALL.H"
#include "MATH.H"

typedef struct BSP_Node {
	BSP_Node* left {0};
	BSP_Node* right {0};
	Wall data;
} BSP_Node;

BSP_Node* BSP_allocateNode (); // Allocates and Initializes a new BSP_Node struct
void BSP_setClipWalls (Wall* walls);
void BSP_setPlayerPosition (vec2D* pos);
void BSP_setPlayerRotation (mat2* m);
void BSP_tadWall (Wall w); // Clips the Wall against ClipWalls and draws the End Result (if any)
int  BSP_countNodes (BSP_Node* start);
int  BSP_countDepth (BSP_Node* root);
void BSP_freeTree (BSP_Node* root); // Frees up all the memory allocated by the Nodes in the Tree

BSP_Node* BSP_insertWall (BSP_Node* node,Wall w); // Inserts a Wall into the BSP Tree
void BSP_drawTree (BSP_Node* start); // Draws the Tree from start down 
