// Graphics Prg 1 for Aaron Nguyen

/***********************************************************
drawing.h

This header file contains the initializtion function alls and set-ups
to draw objects to the cavas using OpenGL. The initialization defines the 
helper functions to draw the snowman using display lists. The header file also 
includes the definition to draw the snowflake and text onto the canvas. 



************************************************************/

#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>            /* Needed to perform trigonometric calculations */
#include <string.h>         /* Needed for string operations to print message onto canvas*/

static unsigned int snowman;                /* Display List base start value */
static unsigned int list[3] = {0, 1, 2};    /* Initial array of list offsets */


/***********************************************************
drawSnowmanSetup

This function is a helper function to setup display list to 
draw the snowman. This function sets of up display units to draw
the base, middle, and top of the snowman. This method was primarily
done to optimize the drawing process as the drawing of the snowman
will remain static and unchanges in any manner.

************************************************************/
static void drawSnowmanSetup(void)
{
    // Return the first of an available block of three successive  list indexes - 
	//to be used as base value.
	snowman = glGenLists(3);

	// Create a display list offset 0 from the base value.
    // Display list to draw the base box of the snowman
	glNewList(snowman, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(220.0, 1.0);
            glVertex2f(400.0, 1.0);
            glVertex2f(400.0, 181.0);
            glVertex2f(220.0, 181.0);
        glEnd();
	glEndList();

	// Create a display list offset 1 from the base value.
    // Display list to draw the middle box of the snowman
	glNewList(snowman + 1, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(250.0, 181.0);
            glVertex2f(370.0, 181.0);
            glVertex2f(370.0, 301.0);
            glVertex2f(250.0, 301.0);
        glEnd();
	glEndList();

	// Create a display list offset 2 from the base value.
    // Dispay list to draw the top box of the snowman
	glNewList(snowman + 2, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(280.0, 301.0);
            glVertex2f(340.0, 301.0);
            glVertex2f(340.0, 361.0);
            glVertex2f(280.0, 361.0);
        glEnd();
	glEndList();
}


/***********************************************************
EXTRA CREDIT
drawSnowmanBonusSetup

This function is a helper function to setup display list to 
draw the snowman. This function sets of up display units to draw
the base, middle, and top of the snowman in an octagon shape. 
This method was primarily done to optimize the drawing process as the 
drawing of the snowman will remain static and unchanges in any manner.

************************************************************/
static void drawSnowmanBonusSetup(void)
{
    // Return base start value with range 3
	snowman = glGenLists(3);

	// Create a display list offset 0 from the base value.
    // Display list to draw the base octagon of the snowman
	glNewList(snowman, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(265.0, 1.0);
            glVertex2f(355.0, 1.0);
            glVertex2f(400.0, 46.0);
            glVertex2f(400.0, 136.0);
            glVertex2f(355.0, 181.0);
            glVertex2f(265.0, 181.0);
            glVertex2f(220.0, 136.0);
            glVertex2f(220.0, 46.0);
        glEnd();
	glEndList();

	// Create a display list offset 1 from the base value.
    // Display list to draw the middle octagon of the snowman
	glNewList(snowman + 1, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(280.0, 181.0);
            glVertex2f(340.0, 181.0);
            glVertex2f(370.0, 211.0);
            glVertex2f(370.0, 271.0);
            glVertex2f(340.0, 301.0);
            glVertex2f(280.0, 301.0);
            glVertex2f(250.0, 271.0);
            glVertex2f(250.0, 211.0);
        glEnd();
	glEndList();

	// Create a display list offset 2 from the base value.
    // Display list to draw the top octagon of the snowman
	glNewList(snowman + 2, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(295.0, 301.0);
            glVertex2f(325.0, 301.0);
            glVertex2f(340.0, 316.0);
            glVertex2f(340.0, 346.0);
            glVertex2f(325.0, 361.0);
            glVertex2f(295.0, 361.0);
            glVertex2f(280.0, 346.0);
            glVertex2f(280.0, 316.0);
        glEnd();
	glEndList();
}


/***********************************************************
drawSnowflake

This helper function is used to draw the snowflake onto the canvas.
Each snowflake drawn by this function consist of three interecting lines
that are sperated by 60 degrees. The starting position of the snowflake
is defined at the top of the canvas. This heper function takes 2 inputs.

INPUTS
* int x
Defines the starting x position of the snowflake

* int offset 
Defines the offset position from the starting position of the snowflake

************************************************************/
void drawSnowflake(int x,int offset)
{	
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
        glVertex2i(x, 600 - offset);
        glVertex2i(x + 25, 600 - 25*sqrt(3) - offset);
        glVertex2i(x + 25/2 - 25, 600 - 25*sqrt(3)/2 - offset);
        glVertex2i(x + 25/2 + 25, 600 - 25*sqrt(3)/2 - offset);
        glVertex2i(x + 25, 600 - offset);
        glVertex2i(x, 600 - 25*sqrt(3) - offset);
    glEnd();
}

/***********************************************************
EXTRA 
textOutput

This helper function is used to output text onto the canvas. 
This was primarily done to work around to prompt the user to press
a key to start animations when the animation was done.
The function uses a bitmap representation to output text onto the canvas. 
This helper function take 3 inputs

INPUTS
*int x
Defines the starting x position of the text

*int y
Defines the starting y position of the text

*char *string
Pointer that references the message 

************************************************************/
void textOutput(int x, int y, char *string)
{
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(x, y);                /* Determine position of the message onto canvas */
    int len = (int)strlen(string);      /* Determine the length of the message */
    for (int i = 0; i < len; i++)       /* Print message using Bitmap representation */
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
    }
}