// Graphics Prg 1 for Aaron Nguyen

/***********************************************************
source.cpp

This file contains the main functionality and function definitions
used to create the canvas using OpenGL. The source file consists
of function calls to draw the snowman, snowflake, and text within the
canvas and terminal. The source file  also contains a frame-by-frame 
based animations to move the snowflakes up and down using timer-based functions. 
The animations will by directed by keyboard callback functions to initiate the 
timer-based animations. 

************************************************************/

#include <stdio.h>					/* Needed for standard I/O to the terminal*/
#include "drawing.h"				/* Includes helper functions to draw objects onto the canvas*/
#include "OpenGL445Setup-aug24.h"	/* Include configurations settings for OpenGL and GLUT*/

int y_pos;							/* y value to offset snowflake */
int units;							/* units to add to offset */
bool drawStart;						/* bool to determine start of animation */


/***********************************************************
display_func()

This function is used to draw objects onto the canvas. The function
will draw the snowman, snowflake, and text onto the canvas with a black
background. The snowman that will be drawn will be defined in main()
depending on what setup was called from drawing.h

EXTRA CREDIT
The snowflake to the right of the canvas 75 units away from the right side
was added to this function, which will be used to redraw the snowflake when 
the animation starts based on the offset y position.

************************************************************/
void display_func()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glListBase(snowman);			/* Specify array of offsets by adding to base "snowman" */
	glCallLists(3, GL_INT, list);	/* Calls display list based on offsets in array "list"*/

	drawSnowflake(75,y_pos);		/* Draw snowflake to the left of the canvas */
	drawSnowflake(525,y_pos);		/* Draw snowflake to the right of the canvas */

	if (!drawStart) 				/* Draw text with message only if animation has not started*/
	textOutput(240,400,"Press any key to start animation");

	glFlush();						/* Clear Buffer */
}


/***********************************************************
timer_func()

This function determines the position of the snowflake and redraw
the canvas using frame-by-frame animations. This function will only
execute if the user enters a key and initiates the animation. The timer
will recalcualte the y position of the snowflake and redraw the canvas on 
a 27 msec timer. This function also determines the ending positions of the 
animation and turns off the timer at the end of the animation.

INPUT
* int val
ID of the timer function


EXTRA CREDIT
When the user starts the animation after the snowflake reaches the bottom of 
the canvas, the timer_func will execute again and move the snowflake up towards
the top of the canvas. This function will outline the ending location of the animation
to restart the canvas and animation to the initial canvas of the program. 

************************************************************/
void timer_func(int val)
{
	if (drawStart)
	{
		y_pos += units;						/* Move snowflake based on units */
		glutPostRedisplay();				/* Redraw canvas */
		glutTimerFunc(27,timer_func,1);		/* Call timer_func */

		if (units > 0)								/* If snowflake is moving down*/
		{
			if ((600 - 25*sqrt(3) - y_pos) <= 4)	/* Ending location of snowflake*/
			{
				y_pos = y_pos;						/* Stop snowflake movement */
				drawStart = false;					/* Stop timer_func	*/
			}
		}
		else 										/* If snowflake is moving up*/
		{
			if ((600 - y_pos) >= 596)				/* Ending location of snowflake*/
			{
				y_pos = y_pos;						/* Stop snowflake movement */
				drawStart = false;					/* Stop timer_func */
			}
		}
	}
}


/***********************************************************
keyInput_func()

This function is used to initiate the animations of the snowflakes
using key press events. Any key will initiate the animation of the
snowflakes by initating the timer_func(). 

INPUTS
*unsigned char key
Defines the key used to initiate the key press event handler. This is not used.

*int x
Defines the x position of the cursor at the time of key press. This is not used.

*int y
Defines the y position of the cusor ar the time of key press. This is not used.

EXTRA CREDIT
When the user starts the animation after the snowflake reaches the bottom of 
the canvas, the keypress event handler will determine when the animation will start
and what direction the snowflake will move during the animation. This function will 
reverse the direction on keypress to allow for starting and restarting the animation. 

************************************************************/
void keyInput_func(unsigned char key, int x, int y)
{
	if (!drawStart)
	{
		units *= -1;						/* Reverse direction of snowflake movement*/
		drawStart = true;					/* Initiate start of animation*/
		glutTimerFunc(27,timer_func,1);		/* Start timer_func to start animation*/
	}
}

#define canvas_Width 600 			/* width of canvas */
#define canvas_Height 600 			/* height of canvas */
char canvas_Name[]= "Program 1"; 	/* name in window top */


/***********************************************************
main()

This function is primary used to initiate settings of the canvas.
The function will define the tye of snowman used within the canvas
and units that will move the y position of the snowflakes. Main will 
also initiate the the glutDisplayFunc to draw onto the canvas and 
glutKeyboardFunc to register the keyboard event handler. 

EXTRA CREDIT
The type of snowman is defined in this function that will be drawn onto
the canvas. It will set up display list to draw either the box snowman or
the octagon snowman depending on the setup function used from drawing.h

************************************************************/
int main(int argc, char ** argv)
{

	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	
	// drawSnowmanSetup();					/* Set up display list to draw box snowman */
	drawSnowmanBonusSetup();				/* Set up display list to draw octagon snowman */

	printf("Press key to start animation. \n");

	y_pos = 0;								/* Init y position offset for snowflake */
	units = -4;								/* Init unit for offset value */
	drawStart = false;						/* Init boolean for animation */

	glutDisplayFunc(display_func); 			/* registers the display callback */
	glutKeyboardFunc(keyInput_func);		/* registers the keyboard callback */
	glutMainLoop(); 						/* execute until killed */
	return 0;
}