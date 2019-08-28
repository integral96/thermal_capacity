#pragma once
#include <GL/glut.h>
#include "button.h"

struct Mouse 
{
	int x;		/*	the x coordinate of the mouse cursor	*/
	int y;		/*	the y coordinate of the mouse cursor	*/
	int lmb;	/*	is the left button pressed?		*/
	int mmb;	/*	is the middle button pressed?	*/
	int rmb;	/*	is the right button pressed?	*/
    int xpress; /*	stores the x-coord of when the first button press occurred	*/
	int ypress; /*	stores the y-coord of when the first button press occurred	*/
};
typedef struct Mouse Mouse;
Mouse TheMouse = {0,0,0,0,0,0,0};
typedef struct Button Button;

int ButtonClickTest(Button* b,int x,int y) 
{
	if( b) 
	{
		if( x > b->x      && 
			x < b->x+b->w &&
			y > b->y      && 
			y < b->y+b->h ) {
				return 1;
		}
	}
    return 0;
}
void ButtonRelease(Button *b,int x,int y)
{
	if(b) 
	{
		if( ButtonClickTest(b,TheMouse.xpress,TheMouse.ypress) && 
			ButtonClickTest(b,x,y) )
		{
			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}
    b->state = 0;
	}
}
void ButtonPress(Button *b,int x,int y)
{
	if(b)
	{
		if( ButtonClickTest(b,x,y) )
		{
			b->state = 1;
		}
	}
}
void ButtonPassive(Button *b,int x,int y)
{
	if(b)
	{
		if( ButtonClickTest(b,x,y) )
		{
			if( b->highlighted == 0 ) {
				b->highlighted = 1;
				glutPostRedisplay();
			}
		}
		else
        if( b->highlighted == 1 )
		{
			b->highlighted = 0;
			glutPostRedisplay();
		}
	}
}

