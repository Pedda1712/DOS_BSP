#include "BSP.H"
#include "WALL.H"
#include "MATH.H"
#include "KEY_INPUT.H"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Wall* clip_Walls;
vec2D* player_pos;
mat2* player_rot;

void BSP_setClipWalls(Wall* walls){clip_Walls = walls;}
void BSP_setPlayerPosition(vec2D* pos){player_pos = pos;}
void BSP_setPlayerRotation(mat2* m){player_rot = m;}

BSP_Node* BSP_allocateNode(){
	BSP_Node* ret = (BSP_Node*)malloc(sizeof(BSP_Node));
	ret->left = 0;
	ret->right = 0;
	return ret;
}

void BSP_tadWall(Wall w){
	moveWall(&w,*player_pos);
	rotateWall(&w,player_rot);

	bool draw = clipWallAgainstPlane(&w,&clip_Walls[0]);;
	if(draw)
		draw = clipWallAgainstPlane(&w,&clip_Walls[1]);
	if(draw)
		draw = clipWallAgainstPlane(&w,&clip_Walls[2]);
	if(draw)
		draw = clipWallAgainstPlane(&w,&clip_Walls[3]);
	if(draw)
		drawWall(&w);
}

BSP_Node* BSP_insertWall (BSP_Node* node, Wall w){

	if (node == 0) { //Wenn hier nichts ist, fuege einen neuen Knoten ein
		BSP_Node* temp = BSP_allocateNode();
		temp->data = w;
		return temp;
	}

	Wall potWall1 = w;
	Wall potWall2 = w;
	bool isSliceAble = sliceWallWithPlane(&potWall1,&potWall2,&node->data);

	if(isSliceAble){
		vec2D pW1_to_data = {node->data.a.x - potWall1.a.x,node->data.a.y - potWall1.a.y};
		fp pW1_dot = node->data.n.x * pW1_to_data.x + node->data.n.y * pW1_to_data.y;

		vec2D pW2_to_data = {node->data.a.x - potWall2.b.x,node->data.a.y - potWall2.b.y};
		fp pW2_dot = node->data.n.x * pW2_to_data.x + node->data.n.y * pW2_to_data.y;

		if(pW1_dot > 0){
			node->left  = BSP_insertWall(node->left,potWall1);
			node->right = BSP_insertWall(node->right,potWall2);
		}else if (pW2_dot > 0){
			node->left  = BSP_insertWall(node->left,potWall2);
			node->right = BSP_insertWall(node->right,potWall1);
		}
	}else{
		vec2D w_to_data = {node->data.a.x - potWall1.a.x,node->data.a.y - potWall1.a.y};
		fp dot = node->data.n.x * w_to_data.x + node->data.n.y * w_to_data.y;

		if(dot > 0){
			node->left = BSP_insertWall(node->left,potWall1);
		}else if (dot < 0){
			node->right = BSP_insertWall(node->right,potWall1);
		}

	}
	return node;
}

int BSP_countNodes (BSP_Node* start){
	if(start == 0) return 0;
	return 1 + BSP_countNodes(start->left) + BSP_countNodes(start->right);
}

void BSP_drawTree (BSP_Node* node){

	if(node == 0) return;

	vec2D actual_player_pos = {-player_pos->x,-player_pos->y};

	vec2D player_to_data = {(node->data.a.x-actual_player_pos.x),(node->data.a.y-actual_player_pos.y)};
	fp dot = (((player_to_data.x * node->data.n.x)) + ((player_to_data.y * node->data.n.y)));

	if(dot <= 0){ // If the player is behind the wall
		BSP_drawTree(node->left);
		BSP_tadWall(node->data);
		BSP_drawTree(node->right);
	}else if (dot > 0){ // If the player is in front of the wall
		BSP_drawTree(node->right);
		BSP_tadWall(node->data);
		BSP_drawTree(node->left);
	}
}

int BSP_countDepth (BSP_Node* root){

	if (root == 0)return 0;

	int leftH = BSP_countDepth(root->left);
	int rightH = BSP_countDepth(root->right);

	if(leftH > rightH){
		return 1 + leftH;
	}else{
		return 1 + rightH;
	}

}

void BSP_freeTree (BSP_Node* root){
	if(root == 0)return;

	BSP_freeTree(root->left);
	BSP_freeTree(root->right);
	free(root);
}
